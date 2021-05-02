#include "compressor.hpp"

compressor::compressor() :
    packed_buf(BLOCK_SIZE),
    uncompressed_buf(BLOCK_SIZE),
    compressed_buf(BLOCK_SIZE)
{}

int compressor::compressBlock(std::ifstream& input_fs, std::ofstream& output_fs, std::size_t input_size)
{
    input_fs.read((char*)uncompressed_buf.data(), input_size);
    compress(uncompressed_buf, input_size, compressed_buf);
    std::size_t codon_count = compressed_buf.size();
    if (compressed_buf.size() % 2 != 0)
        compressed_buf.push_back(0u);
    std::size_t packed_size = compressed_buf.size() / 2 * 3;
    pack(compressed_buf, packed_buf);
    std::uint32_t checksum = crc32(packed_buf.data(), packed_size);
    output_fs.write((char*)&codon_count, sizeof(codon_count));
    output_fs.write((char*)&packed_size, sizeof(packed_size));
    output_fs.write((char*)packed_buf.data(), packed_size);
    output_fs.write((char*)&checksum, sizeof(checksum));
    compressed_buf.clear();
    return OK;
}

int compressor::decompressBlock(std::ifstream& input_fs, std::ofstream& output_fs)
{
    std::size_t codon_count = 0, packed_size = 0;
    std::uint32_t read_checksum = 0;
    input_fs.read((char*)&codon_count, sizeof(codon_count));
    input_fs.read((char*)&packed_size, sizeof(packed_size));
    if (packed_size > packed_buf.size())
            packed_buf.resize(packed_size);
    input_fs.read((char*)packed_buf.data(), packed_size);
    input_fs.read((char*)&read_checksum, sizeof(read_checksum));
    std::uint32_t my_checksum = crc32(packed_buf.data(), packed_size);
    if (read_checksum != my_checksum)
        return ERR_CRC;
    unpack(codon_count, packed_buf, compressed_buf);
    int state = decompress(compressed_buf.data(), codon_count, uncompressed_buf);
    if (state != OK)
        return state;
    output_fs.write((char*)uncompressed_buf.data(), uncompressed_buf.size());
    return OK;
}

int compressor::compressFile(std::ifstream& input_fs, std::ofstream& output_fs, std::size_t file_size)
{
    std::size_t whole_blocks_count = file_size / BLOCK_SIZE;
    std::size_t remainder_size = file_size % BLOCK_SIZE;
    std::size_t total_blocks_count = whole_blocks_count + !!(remainder_size);
    output_fs.write((char*)&total_blocks_count, sizeof(total_blocks_count));
    for (std::size_t i = 0; i < whole_blocks_count; i++)
        compressBlock(input_fs, output_fs, BLOCK_SIZE);
    if (remainder_size != 0)
        compressBlock(input_fs, output_fs, remainder_size);
    return OK;
}

int compressor::decompressFile(std::ifstream& input_fs, std::ofstream& output_fs)
{
    std::size_t blocks_count = 0;
    input_fs.read((char*)&blocks_count, sizeof(blocks_count));
    int state = OK;
    for (std::size_t i = 0; i < blocks_count; i++) {
        state = decompressBlock(input_fs, output_fs);
        if (state != OK)
            return state;
    }
    return OK;
}
