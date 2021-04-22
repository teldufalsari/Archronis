#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include "lzw.hpp"
namespace fs = std::filesystem;


int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " [filename]" << std::endl;
        return ERR_NO_FILE;
    }
    fs::path filepath(argv[1]); 
    if (!fs::exists(filepath)) {
        std::cout << "File " << filepath << " does not exist" << std::endl;
        return ERR_NO_FILE;
    }
    fs::file_status this_status = fs::status(filepath);
    if (this_status.type() != fs::file_type::regular) {
        std::cout << "File " << filepath << " cannot be compressed: not a regular file" << std::endl;
        return ERR_NOT_REG;
    }
    std::uintmax_t file_size = fs::file_size(filepath);
    std::ifstream input_fs(filepath);
    if (!input_fs.is_open()) {
        std::cout << "Could not open file " << filepath << std::endl;
        return ERR_FS_ERROR;
    }
    std::ofstream output_fs(std::string(argv[1]) + std::string(".cmp"));
    if (!output_fs.is_open()) {
        std::cout << "Could not create file for writing down" << std::endl;
        return ERR_OPEN;
    }
    int state = compress_all(input_fs, output_fs, file_size);
    if (state != OK)
        return state;
    input_fs.close();
    output_fs.close();
    std::error_code erc;
    fs::permissions(filepath, this_status.permissions(), erc);
    if (erc.value()) {
        std::cout << "Could not set file permissions: " << erc.message() << std::endl;
        return erc.value();
    }
    return 0;
}