#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include "lzw.hpp"
namespace fs = std::filesystem;

const std::uintmax_t RAT_BLOCK_SIZE = 4096UL * 32UL;
const char RAT_SIGNATURE[8] = "krysa02";

struct rat_metadata {
    fs::file_status status;
    std::uintmax_t size;
    fs::file_time_type last_mod_time;
    std::size_t name_size;
};

int rat_pack(int count, char* names[]);
int rat_unpack(const char* name);
int gather_metadata(rat_metadata* buffer, std::string* names, int count);

int main(int argc, char* argv[])
{
    if (argc < 3) {
        std::cout << "Usage:\n"
            << argv[0] << " pack [file 1] [file 2] ... [output file]\n"
            << argv[0] << " unpack [input file]" << std::endl;
        return ERR_NO_COMMAND;
    }

    if (std::string(argv[1]) == "pack") {
        if (argc < 4) {
            std::cout << "Output archive name not specified" << std::endl;
            return ERR_NO_FILE;
        }
        return rat_pack(argc - 2, argv + 2);
    }
    if (std::string(argv[1]) == "unpack")
        return rat_unpack(argv[2]);
    std::cout << "Unknown command '" << argv[1] << '\'' << std::endl;
    return ERR_NO_COMMAND;
}

int rat_pack(int count, char* names[])
{
    int file_count = count - 1;
    auto file_names = new std::string[file_count];
    for (int i = 0; i < file_count; i++)
        file_names[i] = names[i];
    std::string arch_name(names[file_count]);
    for (int i = 0; i < file_count; i++)
        if (!fs::exists(names[i])) {
            std::cout << "Could not create archive: file '" << file_names[i] << "' does not exist" << std::endl;
            return ERR_NO_FILE;
        }
    auto metadata = new rat_metadata[file_count];
    // add try/catch block here
    int state = gather_metadata(metadata, file_names, file_count);
    if (state != OK) {
        std::cout << "Could not create archive: only regular files are allowed" << std::endl;
        delete[] metadata;
        return state;
    }
    std::ofstream output_fs(arch_name);
    if (!output_fs.is_open()) {
        std::cout << "Could not open file '" << arch_name << "' for writing the archive" << std::endl;
        return ERR_OPEN;
    }
    output_fs.write(RAT_SIGNATURE, sizeof(RAT_SIGNATURE));
    output_fs.write((char*)&file_count, sizeof(file_count));
    auto data_buffer = new char[RAT_BLOCK_SIZE];
    for (int i = 0; i < file_count; i++) {
        output_fs.write((char*)&metadata[i], sizeof(rat_metadata));
        output_fs.write(file_names[i].c_str(), file_names[i].size());
        std::ifstream input_fs(file_names[i]);
        if (!input_fs.is_open()) {
            std::cout << "Could not open file '" << file_names[i] << "' for reading" << std::endl;
            delete[] metadata;
            return ERR_READ;
        }
        std::uintmax_t whole_blocks_count = metadata[i].size / RAT_BLOCK_SIZE;
        std::uintmax_t remainder_size = metadata[i].size % RAT_BLOCK_SIZE;
        for (std::uintmax_t j = 0; j < whole_blocks_count; j++) {
            // Wrap read/write in a function and add stream state checks
            input_fs.read(data_buffer, RAT_BLOCK_SIZE);
            output_fs.write(data_buffer, RAT_BLOCK_SIZE);
        }
        if (remainder_size != 0) {
            // Wrap read/write in a function and add stream state checks
            // And add read/write checks everywhere
            input_fs.read(data_buffer, remainder_size);
            output_fs.write(data_buffer, remainder_size);
        }
        input_fs.close();
    }
    delete[] file_names;
    delete[] data_buffer;
    delete[] metadata;
    output_fs.close();
    return OK;
}

int gather_metadata(rat_metadata* buffer, std::string* names, int count)
{
    int ret = OK;
    for (int i = 0; i < count; i++) {
        buffer[i].status = fs::status(names[i]);
        if (buffer[i].status.type() != fs::file_type::regular) {
            std::cout << "File '" << names[i] << "' is not a regular file" << std::endl;
            ret = ERR_NOT_REG;
            continue;
        }
        buffer[i].size = fs::file_size(names[i]);
        buffer[i].last_mod_time = fs::last_write_time(names[i]);
        buffer[i].name_size = names[i].size();
    }
    return ret;
}

int rat_unpack(const char* name)
{
    std::string arch_name(name);
    if (!fs::exists(arch_name)) {
        std::cout << "File '" << arch_name << "' does not exist" << std::endl;
        return ERR_NO_FILE;
    }
    if (!fs::is_regular_file(arch_name)) {
        std::cout << "File '" << arch_name << "' can't be an archive file: not a regular file" << std::endl;
        return ERR_NOT_REG;
    }
    std::ifstream input_fs(arch_name);
    if (!input_fs.is_open()) {
        std::cout << "Could not open file '" << arch_name << "' for extracting" << std::endl;
        return ERR_OPEN;
    }
    std::uintmax_t signature = 0;
    input_fs.read((char*)&signature, sizeof(signature));
    if (signature != *((const std::uintmax_t*)&RAT_SIGNATURE)) {
        std::cout << "File '" << arch_name << "' is not a valid archive file" << std::endl;
        return ERR_NOT_ARCH;
    }
    int file_count = 0;
    input_fs.read((char*)&file_count, sizeof(file_count));
    rat_metadata cur_metadata = {};
    auto data_buffer = new char[RAT_BLOCK_SIZE];
    for (int i = 0; i < file_count; i++) {
        input_fs.read((char*)&cur_metadata, sizeof(cur_metadata));
        if (!input_fs.good()) {
            std::cout << "Could not read more data" << std::endl;
            return ERR_READ;
        }
        auto cur_name_buf = (char*)calloc(cur_metadata.name_size + 1, sizeof(char));
        input_fs.read(cur_name_buf, cur_metadata.name_size);
        /*
         * Good idea to make function "int check(const std::fstream &fs)"
         * Call: int state = (check(input_fs);
         *       if (state) return state;
         * Check function does all printing to std output
         */
        if (!input_fs.good()) {
            std::cout << "Could not read more data: ";
            if (input_fs.bad())
                std::cout << "badbit set" << std::endl;
            else if (input_fs.eof())
                std::cout << "reached end" << std::endl;
            else
                std::cout << "failbit set" << std::endl;
            return ERR_READ;
        }
        cur_name_buf[cur_metadata.name_size] = '\0';
        std::string cur_name(cur_name_buf);
        free(cur_name_buf);
        std::ofstream output_fs(cur_name);
        if (!output_fs.is_open()) {
            std::cout << "Could not create file '" << cur_name << '\'' << std::endl;
            delete[] data_buffer;
            return ERR_OPEN;
        }
        std::uintmax_t whole_blocks_count = cur_metadata.size / RAT_BLOCK_SIZE;
        std::uintmax_t remainder_size = cur_metadata.size % RAT_BLOCK_SIZE;
        for (std::uintmax_t j = 0; j < whole_blocks_count; j++) {
            input_fs.read(data_buffer, RAT_BLOCK_SIZE);
            output_fs.write(data_buffer, RAT_BLOCK_SIZE);
        }
        if (remainder_size != 0) {
            // It's the fourth time this r/w is taken
            // i strongly recommend using a function
            input_fs.read(data_buffer, remainder_size);
            output_fs.write(data_buffer, remainder_size);
        }
        output_fs.close();
        // set metadata
        std::error_code errc;
        fs::permissions(cur_name, cur_metadata.status.permissions(), errc);
        if (errc.value())
            std::cout << "Failed to modify access permissions for file '" << cur_name << '\'' << std::endl;
        fs::last_write_time(cur_name, cur_metadata.last_mod_time, errc);
        if (errc.value())
            std::cout << "Failed to modify last modification time for file '" << cur_name << '\'' << std::endl;
    }
    delete[] data_buffer;
    return OK;
}
