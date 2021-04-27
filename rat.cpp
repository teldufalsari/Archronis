#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include "lzw.hpp"
namespace fs = std::filesystem;

const char COMPRESSED_SIGNATURE[8] = "rat03cp";
const char ARCHIVED_SIGNATURE[8] = "rat03ar";

struct rat_metadata {
    fs::file_status status;
    std::uintmax_t size;
    fs::file_time_type last_mod_time;
    std::size_t name_size;
};

int rat_pack(int count, char* names[]);
int rat_unpack(const char* name);
int gather_metadata(tld::vector<rat_metadata>& meta, const tld::vector<std::string> names);
int compress_file(const std::string& name, const rat_metadata& metadata, std::ofstream& output_fs);

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
    int input_count = count - 1;
    tld::vector<std::string> input_names;
    for (int i = 0; i < input_count; i++)
        input_names.push_back(names[i]);
    std::string arch_name(names[input_count]);
    tld::vector<rat_metadata> metadata_v;
    // add try/catch block here
    int state = gather_metadata(metadata_v, input_names);
    if (state != OK) {
        std::cout << "Could not create an archive due to previous errors" << std::endl;
        return state;
    }
    std::ofstream output_fs(arch_name);
    if (!output_fs.is_open()) {
        std::cout << "Could not open file '" << arch_name << "' for writing the archive" << std::endl;
        return ERR_OPEN;
    }
    output_fs.write(COMPRESSED_SIGNATURE, sizeof(COMPRESSED_SIGNATURE));
    output_fs.write((char*)&input_count, sizeof(input_count));
    for (std::size_t i = 0; i < input_names.size(); i++) {
        state = compress_file(input_names[i], metadata_v[i], output_fs);
        if (state != OK) {
            std::cout << "Could not compress file '" << input_names[i] << '\'' << std::endl;
            return state;
        }
    }
    return OK;
}

int compress_file(const std::string& name, const rat_metadata& metadata, std::ofstream& output_fs)
{
    std::ifstream input_fs(name);
    if (!input_fs.is_open()) {
        return ERR_OPEN;
    }
    output_fs.write((const char*)&metadata, sizeof(metadata));
    output_fs.write(name.c_str(), name.size());
    return compress_all(input_fs, output_fs, metadata.size);
}

int gather_metadata(tld::vector<rat_metadata>& meta, const tld::vector<std::string> names)
{
    int ret = OK;
    rat_metadata buffer = {};
    for (std::size_t i = 0; i < names.size(); i++) {
        if (!fs::exists(names[i])) {
            std::cout << "File '" << names[i] << "' does not exist" << std::endl;
            ret = ERR_NO_FILE;
            continue;
        }
        buffer.status = fs::status(names[i]);
        if (buffer.status.type() != fs::file_type::regular) {
            std::cout << "File '" << names[i] << "' is not a regular file" << std::endl;
            ret = ERR_NOT_REG;
            continue;
        }
        buffer.size = fs::file_size(names[i]);
        buffer.last_mod_time = fs::last_write_time(names[i]);
        buffer.name_size = names[i].size();
        meta.push_back(buffer);
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
    if (signature != *((const std::uintmax_t*)&COMPRESSED_SIGNATURE)) {
        std::cout << "File '" << arch_name << "' is not a valid archive file" << std::endl;
        return ERR_NOT_ARCH;
    }
    int file_count = 0;
    input_fs.read((char*)&file_count, sizeof(file_count));
    rat_metadata cur_metadata = {};
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
            return ERR_OPEN;
        }
        int state = decompress_all(input_fs, output_fs);
        if (state != OK)
            return state;
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
    return OK;
}
