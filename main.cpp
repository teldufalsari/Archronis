#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include "lzw.hpp"
#include "compressor.hpp"
namespace fs = std::filesystem;
/**
 * @file main.cpp
 * @brief Main control logic for the program
 */

const char COMPRESSED_SIGNATURE[8] = "archr04";

/// 
struct arc_metadata {
    fs::file_status status;
    std::uintmax_t size;
    fs::file_time_type last_mod_time;
    std::size_t name_size;
};

std::string ArcStrerror(const std::string& file_name, int err_code)
{
    switch (err_code) {
    case ERR_NO_COMMAND:
        return "File '" +  file_name + "' does not exist";
    case ERR_OPEN:
        return "Could not open file '" + file_name + '\'';
    case ERR_NOT_REG:
        return "File '" + file_name + "' is not regular";
    case ERR_ALLOC:
        return "Could not allocate memory to handle file '" + file_name + '\'';
    case ERR_READ:
        return "Could not read data from file '" + file_name + '\'';
    case ERR_FSTREAM:
        return "Could not process file '" + file_name + "' due to file i/o error";
    case ERR_DECODE:
        return "Could not decode file '" + file_name + "', it appears to be corrupted";
    case ERR_NOT_ARCH:
        return "File '" + file_name + "' is not signed as an archive, I won't touch it";
    case ERR_CRC:
        return "Checksum error: file '" + file_name + "' in the arcive appears to be corrupted";
    case ERR_CREATE:
        return "Could not create file to extract data from '" + file_name + '\'';
    default:
        return "Unknown error occured while processing file '" + file_name + '\'';
    }
}

/**
 * @brief Find file and try to open it as a regular for reading
 * @param file_name path to file
 * @param file_stream input stream to associate file with
 * @return Zero on success, otherwise non-zero error code
 */
int TryOpenArchive(const std::string& file_name, std::ifstream& file_stream)
{
    if (!fs::exists(file_name)) {
        std::cout << "File '" << file_name << "' does not exist" << std::endl;
        return ERR_NO_FILE;
    }
    if (!fs::is_regular_file(file_name)) {
        std::cout << "File '" << file_name << "' can't be an archive file: not a regular file" << std::endl;
        return ERR_NOT_REG;
    }
    file_stream.open(file_name, std::ios::in | std::ios::binary);
    if (!file_stream.is_open()) {
        std::cout << "Could not open file '" << file_name << "' for extracting" << std::endl;
        return ERR_OPEN;
    }
    return OK;
}

/**
 * @brief Gather metatada of given list of files
 * @param meta vector where mwtadata structires will be stored
 * @param names list of paths to files
 * @return Zero on success, otherwise non-zero error code
 */
int GatherMetadata(tld::vector<arc_metadata>& meta, const tld::vector<std::string> names)
{
    int ret = OK;
    arc_metadata buffer = {};
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
        try {
            buffer.size = fs::file_size(names[i]);
            buffer.last_mod_time = fs::last_write_time(names[i]);
        } catch (std::exception& ex) {
            std::cout << "Could not collect file metadata for file '" << names[i] << ":\n";
            std::cout << ex.what() << std::endl;
            ret = ERR_FS_ERROR;
        }
        buffer.name_size = names[i].size();
        meta.push_back(buffer);
    }
    return ret;
}

/**
 * @brief Open file and write compressed data into output stream associated with archive file
 * @param name path to file
 * @param metadata file metadata for archiving
 * @param output_fs output stream
 * @return Zero on success, otherwise non-zero error code
 */
int CompressFile(const std::string& name, const arc_metadata& metadata, std::ofstream& output_fs)
{
    std::ifstream input_fs(name, std::ios::in | std::ios::binary);
    if (!input_fs.is_open()) {
        return ERR_OPEN;
    }
    output_fs.write((const char*)&metadata, sizeof(metadata));
    output_fs.write(name.c_str(), name.size());
    compressor compr;
    int state = compr.compressFile(input_fs, output_fs, metadata.size);
    if (state == OK && (!input_fs.good() || !output_fs.good()))
        state = ERR_FSTREAM;
    return state;
}

/**
 * @brief Create file from a record in the archive
 * @param input_fs file stream associated with the archive
 * @param compr compressor instance
 * @return Zero on success, otherwise non-zero error code
 */
int DecompressFile(std::ifstream& input_fs, compressor& compr)
{
    arc_metadata cur_metadata = {};
    input_fs.read((char*)&cur_metadata, sizeof(cur_metadata));
    if (!input_fs.good())
        return ERR_FSTREAM;
    auto cur_name_buf = new char[cur_metadata.name_size + 1];
    input_fs.read(cur_name_buf, cur_metadata.name_size);
    if (!input_fs.good()) {
        delete[] cur_name_buf;
        return ERR_FSTREAM;
    }
    cur_name_buf[cur_metadata.name_size] = '\0';
    std::string cur_name(cur_name_buf);
    delete[] cur_name_buf;
    std::ofstream output_fs(cur_name, std::ios::out | std::ios::binary);
    if (!output_fs.is_open())
        return ERR_CREATE;
    int state = compr.decompressFile(input_fs, output_fs);
    input_fs.close();
    if (state == OK && (!input_fs.good() || !output_fs.good()))
        state = ERR_FSTREAM;
    if (state != OK)
        return state;
    try {
        fs::permissions(cur_name, cur_metadata.status.permissions());
        fs::last_write_time(cur_name, cur_metadata.last_mod_time);
    } catch (std::exception& ex) {
        std::cout << "Failed to set proper metadata for file" << cur_name << "\n";
        std::cout << ex.what() << std::endl;
    }
    return state;
}

/**
 * @brief Run Archronis in packing mode
 * @param count number of input files, archive counts
 * @param names array of C-string containng input names and archive name
 * @returns exit code
 */
int PackMode(int count, char* names[])
{
    int input_count = count - 1;
    tld::vector<std::string> input_names;
    for (int i = 0; i < input_count; i++)
        input_names.push_back(names[i]);
    std::string arch_name(names[input_count]);
    tld::vector<arc_metadata> metadata_v;
    int state = GatherMetadata(metadata_v, input_names);
    if (state != OK) {
        std::cout << "Could not create an archive due to previous errors" << std::endl;
        return state;
    }
    std::ofstream output_fs(arch_name, std::ios::out | std::ios::binary);
    if (!output_fs.is_open()) {
        std::cout << "Could not open file '" << arch_name << "' for writing the archive" << std::endl;
        return ERR_OPEN;
    }
    output_fs.write(COMPRESSED_SIGNATURE, sizeof(COMPRESSED_SIGNATURE));
    output_fs.write((char*)&input_count, sizeof(input_count));
    for (std::size_t i = 0; i < input_names.size(); i++) {
        state = CompressFile(input_names[i], metadata_v[i], output_fs);
        if (state != OK) {
            std::cout << ArcStrerror(input_names[i], state) << std::endl;
            return state;
        }
    }
    return OK;
}

/**
 * @brief Run Archronis in unpacking mode
 * @param name C-string - path to archive
 * @return exit code
 */
int UnpackMode(const char* name)
{
    std::string arch_name(name);
    std::ifstream input_fs;
    int state = TryOpenArchive(arch_name, input_fs);
    if (state != OK) {
        std::cout << ArcStrerror(arch_name, state) << std::endl;
        return state;
    }
    std::uintmax_t signature = 0;
    input_fs.read((char*)&signature, sizeof(signature));
    if (signature != *((const std::uintmax_t*)&COMPRESSED_SIGNATURE)) {
        std::cout << ArcStrerror(arch_name, ERR_NOT_ARCH) << std::endl;
        return ERR_NOT_ARCH;
    }
    int file_count = 0;
    input_fs.read((char*)&file_count, sizeof(file_count));
    compressor compr;
    for (int i = 0; i < file_count; i++) {
        state = DecompressFile(input_fs, compr);
        if (state != OK) {
            std::cout << ArcStrerror(arch_name, state) << std::endl;
            return state;
        }
    }
    return OK;
}

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
        return PackMode(argc - 2, argv + 2);
    }
    if (std::string(argv[1]) == "unpack")
        return UnpackMode(argv[2]);
    std::cout << "Unknown command '" << argv[1] << '\'' << std::endl;
    return ERR_NO_COMMAND;
}
