#ifndef RAT_COMPRESSOR_H
#define RAT_COMPRESSOR_H
#include "lib/byte_str.h"
#include "lzw.hpp"
#include "lib/vector.h"

class compressor
{
    // Buffer for data that is compressed and densely packed
    // This kind of data is stored in archives
    byte_str packed_buf;

    // Buffer for raw data that is going to be compressed or written on disk after decompression
    byte_str uncompressed_buf;

    // Buffer for intermediate representation of compressed data as sequences of 16-bit codes
    // Only 12 bit out of 16 are significant
    tld::vector<pos_t> compressed_buf;

public:
    /// Creates compressor with pre-allocated buffers
    compressor();

    /// Default constructor
    ~compressor() = default;

    /**
     * @brief Read data from a stream and write compressed data to the other stream
     * @param input_fs input file stream
     * @param output_fs output file stream
     * @param input_size number of bytes to be read from input
     * @returns Always OK, you need to turn on exceptions for the streams
     */
    int compressBlock(std::ifstream& input_fs, std::ofstream& output_fs, std::size_t input_size);

    /**
     * @brief Read compressed block of data from one stream and write decompressed data to the other stream
     * @param input_fs input file stream
     * @param output_fs output file stream
     * @returns On success zero is returned. Either ERR_CRC or ERR_DECODE if returned if decompression was not successfull
     */
    int decompressBlock(std::ifstream& input_fs, std::ofstream& output_fs);

    /**
     * @brief Read data from a stream and write compressed data to the other stream
     * @param input_fs input file stream
     * @param output_fs output file stream
     * @param input_size number of bytes to be read from input
     * @returns Always OK, you need to turn on exceptions for the streams
     * @details Same as compressBlock, but processes files blockwise (for large files)
     */
    int compressFile(std::ifstream& input_fs, std::ofstream& output_fs, std::size_t file_size);

    /**
     * @brief Read sequence of compressed blocks and write decompressed data to output_fs
     * @param input_fs input file stream
     * @param output_fs output file stream
     */
    int decompressFile(std::ifstream& input_fs, std::ofstream& output_fs);
};

#endif // RAT_COMPRESSOR_H
