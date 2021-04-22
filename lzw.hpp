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

/**
 * @brief Compress block of data
 * @param input Byte array of data to compress
 * @param input_size size of the input array in sizeof(char)
 * @param result Output vector where compressed data is stored
 * @details Data in stored in 16-bit code words.
 * Use function pack() to pack data into array of 12-bit sequences
 * All data that was stored in the output vector is cleared.
 */
void compress(const char* input, size_t input_size, tld::vector<pos_t>& result);

/**
 * @brief Pack compressed data densely
 * @param input Input vector of compressed data
 * @param output Pointer to the array of output data.
 * The requred memory will be allocated, caller should free
 * this memory by calling delete[] operator
 * @return Size of the allocated memory where packed data is stored.
 */
size_t pack(tld::vector<pos_t>& input, unsigned char** output);

/**
 * @brief Decode data compressed by compress() function
 * @param input Array compressed data in 16-bit code words
 * @param input_size Number of elements in the input array
 * (not size in bytes)
 * @param result Output string of decompressed data
 * @return On success, zero is returned.
 * If compressed data is invalid, ERR_DECODE id returned
 * When the function fails to allocate memory, ERR_ALLOC is returned.
 */
int decompress(const pos_t* input, size_t input_size, std::string& result);

/**
 * @brief Unpack data from 12-bit sequences to 16-bit
 * @param packed The array of packed data
 * @param codon_count Number of significant 12-bit words in the array.
 * Number of elements in packed arra must be an even number, even if
 * codon_count is odd.
 * @return The array of unpacked data. The caller must then free the array
 * by calling delete[] operator.
 */
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

/**
 * @brief Check if file pointed by filepath is compressable and open it if it is
 * @param filepath Path to the file
 * @param input_filestream input filestream object that would be opened on success
 * to read from filepath
 * @return On success, zero is returnes.
 * Otherwise, non-zero error word is returned.
 */
int check_and_open(const fs::path& filepath, std::ifstream& input_filestream);

/**
 * @brief Compress file_size bytes from input_fs and write compressed and packed data
 * into output_fs.
 * Number of 12-bit code words is written in first 8 bytes before the data.
 */
int compress_all(std::ifstream& input_fs, std::ofstream& outfupt_fs, std::uintmax_t file_size);

/**
 * @brief Read packed and compressed data from input_fs and write
 * decompressed data intp output_fs.
 * The first 8 bytes of input_fs must be the numbe of 12-bit code words in
 * packed file.
 */
int decompress_all(std::ifstream& input_fs, std::ofstream& outfupt_fs);

#endif // JACKAL_LZW_H
