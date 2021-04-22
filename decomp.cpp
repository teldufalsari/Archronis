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
        std::cout << "File '" << filepath << "' does not exist" << std::endl;
        return ERR_NO_FILE;
    }
    fs::file_status this_status = fs::status(filepath);
    if (this_status.type() != fs::file_type::regular) {
        std::cout << "File '" << filepath << "' cannot be compressed: not a regular file" << std::endl;
        return ERR_NOT_REG;
    }
    std::uintmax_t file_size = fs::file_size(filepath);
    std::ifstream input_fs(filepath);
    if (!input_fs.is_open()) {
        std::cout << "Could not open file '" << filepath << '\'' << std::endl;
        return ERR_FS_ERROR;
    }
    std::ofstream output_fs(std::string(argv[1]) + std::string(".dcp"));
    if (!output_fs.is_open()) {
        std::cout << "Could not create file for writing down" << std::endl;
        return ERR_OPEN;
    }
    int state = decompress_all(input_fs, output_fs);
    input_fs.close();
    output_fs.close();
    if (state != OK) {
        switch (state) {
        case ERR_ALLOC:
            std::cout << "Could not allocate memory for file decoding" << std::endl;
            break;
        
        case ERR_DECODE:
            std::cout << "Could not decompress: the file is possibly corrupted" << std::endl;
        default:
            break;
        }
        return state;
    }
    return 0;
}
