#ifndef RAT_LZW_H
#define RAT_LZW_H
#include <string>
#include "lib/map.h"
#include "lib/vector.h"
#include "lib/byte_str.h"
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
    ERR_NOT_ARCH,
    ERR_CRC
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
void compress(const byte_str& input, size_t input_size, tld::vector<pos_t>& result);

/**
 * @brief Pack compressed data densely
 * @param input Input vector of compressed data
 * @param output Pointer to the array of output data.
 */
size_t pack(tld::vector<pos_t>& input, byte_str& output);

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
int decompress(const pos_t* input, size_t input_size, byte_str& result);

/**
 * @brief Unpack densely packed data
 * @param codon_count number of 12-bit code words in the input sequence
 * @param packed byte string with packed data
 * @param unpacked container where unpacked data are placed
 */
void unpack(std::size_t codon_count, const byte_str& packed, tld::vector<pos_t>& unpacked);

static inline void pack_pair(const pos_t* in_buf, std::byte* out_buf)
{
    auto in_bytes = (const std::byte*)in_buf;
    out_buf[0] = in_bytes[0];
    out_buf[1] = (in_bytes[1] << 4u) | (in_bytes[2] >> 4u);
    out_buf[2] = (in_bytes[2] << 4u) | (in_bytes[3] & std::byte(0x0F));
}

static inline void unpack_pair(const std::byte* in_buf, pos_t* out_buf)
{
    auto out_bytes = (std::byte*)out_buf;
    out_bytes[0] = in_buf[0];
    out_bytes[1] = in_buf[1] >> 4u;
    out_bytes[2] = (in_buf[1] << 4u) | (in_buf[2] >> 4u);
    out_bytes[3] = in_buf[2] & std::byte(0x0F);
}

/**
 * @brief Calculate a bytewise CRC-32 checksum of given array
 * @param data_buf data buffer
 * @param data_size size of given array in bytes
 */
unsigned crc32(const std::byte* data_buf, std::size_t data_size);

/**
 * @brief Generate table for calculating CRC-32 checksum
 * @details It is used by crc32 function and must be used only by it.
 */
static void crc_generate_table(unsigned* table);

#endif // RAT_LZW_H
