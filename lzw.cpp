#include "lzw.hpp"

int decompress(const pos_t* input, size_t input_size, byte_str& result)
{
    tld::vector<byte_str> dict(TABLE_SIZE);
    for (pos_t i = 0; i < 256; i++)
        dict.push_back(byte_str(1, std::byte(i)));
    byte_str w(1, std::byte(input[0]));
    result = w;
    byte_str buffer;
    for (size_t i = 1; i < input_size; i++) {
        pos_t code = input[i];
        if (code < dict.size()) {
            buffer = dict[code];
        } else if (code == dict.size()) {
            buffer = w + w[0];
        } else {
            return ERR_DECODE;
        }
        result.append(buffer);
        if (dict.size() < TABLE_SIZE) {
            byte_str tmp = w + buffer[0];
            dict.push_back(tmp);
        }
        w = buffer;
    }
    return OK;
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
        if (dict.Count(bc) != 0) {
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

/**
 * @brief Generate table for calculating CRC-32 checksum
 * @details It is used by crc32 function and must be used only by it.
 */
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
