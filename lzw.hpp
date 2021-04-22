#ifndef JACKAL_LZW_H
#define JACKAL_LZW_H
#include <string>
#include "lib/map.h"
#include "lib/vector.h"
#include <filesystem>
#include <iostream>
#include <fstream>
namespace fs = std::filesystem;

enum errors {
    OK = 0,
    ERR_NO_COMMAND,
    ERR_NO_FILE,
    ERR_STAT,
    ERR_OPEN,
    ERR_NOT_REG,
    ERR_ALLOC,
    ERR_FS_ERROR,
    ERR_READ,
    ERR_WRITE,
    ERR_DECODE,
    ERR_NOT_ARCH
};

const std::string EMPTY_STR = std::string();
const size_t TABLE_SIZE = 4096;
const size_t BLOCK_SIZE = 4096 * 4;
typedef unsigned short pos_t;

void compress(const char* input, size_t input_size, tld::vector<pos_t>& result);
size_t pack(tld::vector<pos_t>& input, unsigned char** output);
int decompress(const pos_t* input, size_t input_size, std::string& result);
pos_t* unpack(size_t codon_count, const unsigned char* packed);

static inline void pack_pair(const pos_t* in_buf, unsigned char* out_buf)
{
    auto in_bytes = (const unsigned char*)in_buf;
    out_buf[0] = in_bytes[0];
    out_buf[1] = (in_bytes[1] << 4u) + (in_bytes[2] >> 4u);
    out_buf[2] = (in_bytes[2] << 4u) + (in_bytes[3] & 0x0F);
}

static inline void unpack_pair(const unsigned char* in_buf, pos_t* out_buf)
{
    auto out_bytes = (unsigned char*)out_buf;
    out_bytes[0] = in_buf[0];
    out_bytes[1] = in_buf[1] >> 4u;
    out_bytes[2] = (in_buf[1] << 4u) + (in_buf[2] >> 4u);
    out_bytes[3] = in_buf[2] & 0x0F;
}

int check_and_open(const fs::path& filepath, std::ifstream& input_filestream);

int compress_all(std::ifstream& input_fs, std::ofstream& outfupt_fs, std::uintmax_t file_size);

int decompress_all(std::ifstream& input_fs, std::ofstream& outfupt_fs);

#endif // JACKAL_LZW_H
