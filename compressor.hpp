#ifndef RAT_COMPRESSOR_H
#define RAT_COMPRESSOR_H
#include "lib/byte_str.h"
#include "lzw.hpp"
#include "lib/vector.h"

class compressor
{
    byte_str packed_buf;
    byte_str uncompressed_buf;
    tld::vector<pos_t> compressed_buf;

public:
    compressor();
    ~compressor() = default;
    int compressBlock(std::ifstream& input_fs, std::ofstream& output_fs, std::size_t input_size);
    int decompressBlock(std::ifstream& input_fs, std::ofstream& output_fs);
    int compressFile(std::ifstream& input_fs, std::ofstream& output_fs, std::size_t file_size);
    int decompressFile(std::ifstream& input_fs, std::ofstream& output_fs);
};

#endif // RAT_COMPRESSOR_H
