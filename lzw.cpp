#include "lzw.hpp"

int decompress(const pos_t* input, size_t input_size, byte_str& result)
{
    size_t dict_size = 256;
    tld::vector<byte_str> dict(TABLE_SIZE);
    for (pos_t i = 0; i < dict_size; i++)
        dict.push_back(byte_str(1, std::byte(i)));
    byte_str w(1, std::byte(input[0]));
    result = w;
    byte_str buffer;
    for (size_t i = 1; i < input_size; i++) {
        pos_t code = input[i];
        if (code < dict_size) {
            buffer = dict[code];
        } else if (code == dict_size) {
            buffer = w + w[0];
        } else {
            return ERR_DECODE;
        }
        result.append(buffer);
        if (dict_size < TABLE_SIZE) {
            byte_str tmp = w + buffer[0];
            dict.push_back(tmp);
            dict_size++;
            //dict[dict_size++] = new std::string(w + buffer[0]);
        }
        w = buffer;
    }
    return OK;
}

pos_t* unpack(size_t codon_count, const std::byte* packed)
{
    pos_t* output = new pos_t[codon_count + 1];
    for (size_t i = 0, j = 0; i < codon_count; i += 2, j += 3)
        unpack_pair(packed + j, output + i);
    return output;
}

void unpack(std::size_t codon_count, const byte_str& packed, tld::vector<pos_t>& unpacked)
{
    unpacked.resize(codon_count + 1);
    for (std::size_t i = 0, j = 0; i < codon_count; i += 2, j += 3)
        unpack_pair(packed.data() + j, unpacked.data() + i);
}

void compress(const byte_str& input, size_t input_size, tld::vector<pos_t>& result)
{
    size_t dict_size = 256;
    tld::map_t<byte_str, pos_t> dict;
    for (pos_t i = 0; i < dict_size; i++)
        dict[byte_str(1, std::byte(i))] = i;
    byte_str buffer, bc;
    result.clear();
    for (size_t i = 0; i < input_size; i++) {
        std::byte c = input[i];
        bc = buffer + c;
        if ((dict[bc] != 0) || (bc == byte_str(1, std::byte(0)))) {
            buffer = bc;
        } else {
            result.push_back(dict[buffer]);
            if (dict_size < TABLE_SIZE)
                dict[bc] = dict_size++;
            buffer = byte_str(1, c);
        }
    }
    if (!buffer.empty())
        result.push_back(dict[buffer]);
}

size_t pack(tld::vector<pos_t>& input, byte_str& output)
{
    // We will write down an empty triplet
    // It happens when input.size() is an odd number.
    int odd = false;
    if (input.size() % 2) {
        input.push_back(0);
        odd = true;
    }
    std::size_t out_size = input.size() / 2 * 3;
    output.resize(out_size);
    for (std::size_t i = 0, j = 0; i < input.size(); i += 2, j += 3)
        pack_pair(input.data() + i, output.data() + j);
    if (odd == true)
        input.pop_back();
    return out_size;
}


int compress_all(std::ifstream& input_fs, std::ofstream& outfupt_fs, std::uintmax_t file_size)
{
    std::uintmax_t whole_blocks_count = file_size / BLOCK_SIZE;
    std::uintmax_t remainder_size = file_size % BLOCK_SIZE;
    std::uintmax_t total_blocks_count = whole_blocks_count + !!(remainder_size);
    outfupt_fs.write((char*)&total_blocks_count, sizeof(total_blocks_count));
    //char* data_buf = new char[BLOCK_SIZE];
    byte_str data_buf(BLOCK_SIZE, std::byte(0));
    //if (data_buf == nullptr)
    //    return ERR_ALLOC;
    tld::vector<pos_t> compressed_buf;
    compressed_buf.reserve(BLOCK_SIZE / 2);
    byte_str packed_buf(BLOCK_SIZE);
    for (std::uintmax_t i = 0; i < whole_blocks_count; i++) {
        // Wraparound function here
        input_fs.read((char*)data_buf.data(), BLOCK_SIZE);
        compress(data_buf, BLOCK_SIZE, compressed_buf);
        std::uintmax_t codon_count = compressed_buf.size();
        if (compressed_buf.size() % 2 != 0)
            compressed_buf.push_back(0u);
        std::uintmax_t packed_size = compressed_buf.size() / 2 * 3;
        pack(compressed_buf, packed_buf);
        unsigned checksum = crc32(packed_buf.data(), packed_size);
        outfupt_fs.write((char*)&codon_count, sizeof(codon_count));
        outfupt_fs.write((char*)&packed_size, sizeof(packed_size));
        outfupt_fs.write((char*)packed_buf.data(), packed_size);
        outfupt_fs.write((char*)&checksum, sizeof(checksum));
        compressed_buf.clear();
    }
    if (remainder_size != 0) {
        // And here
        input_fs.read((char*)data_buf.data(), remainder_size);
        compress(data_buf, remainder_size, compressed_buf);
        std::uintmax_t codon_count = compressed_buf.size();
        if (compressed_buf.size() % 2 != 0)
            compressed_buf.push_back(0u);
        std::uintmax_t packed_size = compressed_buf.size() / 2 * 3;
        pack(compressed_buf, packed_buf);
        unsigned checksum = crc32(packed_buf.data(), packed_size);
        // Check stream state
        outfupt_fs.write((char*)&codon_count, sizeof(codon_count));
        outfupt_fs.write((char*)&packed_size, sizeof(packed_size));
        outfupt_fs.write((char*)packed_buf.data(), packed_size);
        outfupt_fs.write((char*)&checksum, sizeof(checksum));
    }
    //delete[] packed_buf;
    //delete[] data_buf;
    return OK;
}

int decompress_all(std::ifstream& input_fs, std::ofstream& outfupt_fs)
{
    // As everywhere, read/write checks
    std::uintmax_t blocks_count = 0;
    input_fs.read((char*)&blocks_count, sizeof(blocks_count));
    byte_str packed_buf(BLOCK_SIZE);
    tld::vector<pos_t> compressed_buf(BLOCK_SIZE);
    unsigned read_checksum = 0;
    for (std::uintmax_t i = 0; i < blocks_count; i++) {
        std::uintmax_t codon_count = 0, packed_size = 0;
        input_fs.read((char*)&codon_count, sizeof(codon_count));
        input_fs.read((char*)&packed_size, sizeof(packed_size));
        if (packed_size > packed_buf.size())
            packed_buf.resize(packed_size);
        input_fs.read((char*)packed_buf.data(), packed_size);
        input_fs.read((char*)&read_checksum, sizeof(read_checksum));
        unsigned my_checksum = crc32(packed_buf.data(), packed_size);
        if (read_checksum != my_checksum)
            return ERR_CRC;
        unpack(codon_count, packed_buf, compressed_buf);
        byte_str decompressed_data(BLOCK_SIZE);
        int state = decompress(compressed_buf.data(), codon_count, decompressed_data);
        if (state != OK)
            return state;
        outfupt_fs.write((char*)decompressed_data.data(), decompressed_data.size());
    }
    return OK;
}

static void crc_generate_table(unsigned* table)
{
    for (unsigned i = 0; i < 256; i++) {
        unsigned remainder = i;
        for (unsigned j = 0; j < 8; j++) {
            if ((remainder & 1u) != 0) {
                remainder >>= 1u;
                remainder ^= 0xedb88320u;
            } else {
                remainder >>= 1u;
            }
        }
        table[i] = remainder;
    }
}

unsigned crc32(const std::byte* data_buf, std::size_t data_size)
{
    static unsigned table[256] = {};
    static bool have_table = false;
    if (!have_table) {
        crc_generate_table(table);
        have_table = true;
    }
    unsigned crc = 0xffffffffu;
    std::byte octet = std::byte(0);
    const std::byte* q = data_buf + data_size;
    for (const std::byte* p = data_buf; p < q; p++) {
        octet = *p;
        crc = (crc >> 8) ^ table[(crc & 0xffu) ^ (unsigned)octet];
    }
    return ~crc;
}
