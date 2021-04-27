#include "lzw.hpp"

int decompress(const pos_t* input, size_t input_size, std::string& result)
{
    size_t dict_size = 256;
    std::string** dict =  (std::string**)calloc(TABLE_SIZE, sizeof(std::string*));
    if (dict == nullptr)
        return ERR_ALLOC;
    try {
        for (pos_t i = 0; i < dict_size; i++)
            dict[i] = new std::string(1, i);
    } catch(std::bad_alloc&) {
        for (pos_t i = 0; i < dict_size; i++)
            delete dict[i];
        return ERR_ALLOC;
    }
    std::string w(1, input[0]);
    result = w;
    std::string buffer;
    for (size_t i = 1; i < input_size; i++) {
        pos_t code = input[i];
        if (dict[code] != nullptr) {
            buffer = *dict[code];
        } else if (code == dict_size) {
            buffer = w + w[0];
        } else {
            for (size_t j = 0; j < dict_size; j++)
                delete dict[j];
            free(dict);
            return ERR_DECODE;
        }
        result += buffer;
        try {
            if (dict_size < TABLE_SIZE) {
                dict[dict_size++] = new std::string(w + buffer[0]);
            }
        } catch(std::bad_alloc&) {
            for (pos_t j = 0; j < dict_size; j++)
                delete dict[j];
            return ERR_ALLOC;
        }
        w = buffer;
    }
    for (size_t j = 0; j < dict_size; j++)
        delete dict[j];
    free(dict);
    return OK;
}

pos_t* unpack(size_t codon_count, const unsigned char* packed)
{
    pos_t* output = new pos_t[codon_count + 1];
    for (size_t i = 0, j = 0; i < codon_count; i += 2, j += 3)
        unpack_pair(packed + j, output + i);
    return output;
}

void compress(const char* input, size_t input_size, tld::vector<pos_t>& result)
{
    size_t dict_size = 256;
    tld::map_t<std::string, pos_t> dict;
    for (pos_t i = 0; i < dict_size; i++)
        dict[std::string(1, i)] = i;
    std::string buffer, bc;
    result.clear();
    for (size_t i = 0; i < input_size; i++) {
        char c = input[i];
        bc = buffer + c;
        if ((dict[bc] != 0) || (bc == std::string(1, 0))) {
            buffer = bc;
        } else {
            result.push_back(dict[buffer]);
            if (dict_size < TABLE_SIZE)
                dict[bc] = dict_size++;
            buffer = std::string(1, c);
        }
    }
    if (!buffer.empty())
        result.push_back(dict[buffer]);
}

size_t pack(tld::vector<pos_t>& input, unsigned char** output)
{
    // We will write down an empty triplet
    // It happens when input.size() is an odd number.
    int odd = false;
    if (input.size() % 2) {
        input.push_back(0);
        odd = true;
    }
    size_t out_size = input.size() / 2 * 3;
    *output = new unsigned char[out_size];
    for (size_t i = 0, j = 0; i < input.size(); i += 2, j += 3)
        pack_pair(input.data() + i, (*output) + j);
    if (odd == true)
        input.pop_back();
    return out_size;
}

void pack(tld::vector<pos_t>& input, unsigned char* output)
{
    // We will write down an empty triplet
    // It happens when input.size() is an odd number.
    int odd = false;
    if (input.size() % 2) {
        input.push_back(0);
        odd = true;
    }
    for (size_t i = 0, j = 0; i < input.size(); i += 2, j += 3)
        pack_pair(input.data() + i, output + j);
    if (odd == true)
        input.pop_back();
}

int compress_all(std::ifstream& input_fs, std::ofstream& outfupt_fs, std::uintmax_t file_size)
{
    std::uintmax_t whole_blocks_count = file_size / BLOCK_SIZE;
    std::uintmax_t remainder_size = file_size % BLOCK_SIZE;
    std::uintmax_t total_blocks_count = whole_blocks_count + !!(remainder_size);
    outfupt_fs.write((char*)&total_blocks_count, sizeof(total_blocks_count));
    char* data_buf = new char[BLOCK_SIZE];
    if (data_buf == nullptr)
        return ERR_ALLOC;
    tld::vector<pos_t> compressed_buf;
    compressed_buf.reserve(BLOCK_SIZE / 2);
    uintmax_t packed_buf_size = BLOCK_SIZE / 2;
    auto packed_buf = new unsigned char[packed_buf_size];
    for (std::uintmax_t i = 0; i < whole_blocks_count; i++) {
        // Wraparound function here
        input_fs.read(data_buf, BLOCK_SIZE);
        compress(data_buf, BLOCK_SIZE, compressed_buf);
        std::uintmax_t codon_count = compressed_buf.size();
        if (compressed_buf.size() % 2 != 0)
            compressed_buf.push_back(0u);
        std::uintmax_t packed_size = compressed_buf.size() / 2 * 3;
        if (packed_size > packed_buf_size) {
            delete[] packed_buf;
            packed_buf = new unsigned char[packed_size];
            packed_buf_size = packed_size;
        }
        pack(compressed_buf, packed_buf);
        unsigned checksum = crc32(packed_buf, packed_buf_size);
        outfupt_fs.write((char*)&codon_count, sizeof(codon_count));
        outfupt_fs.write((char*)&packed_size, sizeof(packed_size));
        outfupt_fs.write((char*)packed_buf, packed_size);
        outfupt_fs.write((char*)&checksum, sizeof(checksum));
        compressed_buf.clear();
    }
    if (remainder_size != 0) {
        // And here
        input_fs.read(data_buf, remainder_size);
        compress(data_buf, remainder_size, compressed_buf);
        std::uintmax_t codon_count = compressed_buf.size();
        if (compressed_buf.size() % 2 != 0)
            compressed_buf.push_back(0u);
        std::uintmax_t packed_size = compressed_buf.size() / 2 * 3;
        if (packed_size > packed_buf_size) {
            delete[] packed_buf;
            packed_buf = new unsigned char[packed_size];
            packed_buf_size = packed_size;
        }
        pack(compressed_buf, packed_buf);
        unsigned checksum = crc32(packed_buf, packed_size);
        // Check stream state
        outfupt_fs.write((char*)&codon_count, sizeof(codon_count));
        outfupt_fs.write((char*)&packed_size, sizeof(packed_size));
        outfupt_fs.write((char*)packed_buf, packed_size);
        outfupt_fs.write((char*)&checksum, sizeof(checksum));
    }
    delete[] packed_buf;
    delete[] data_buf;
    return OK;
}

int decompress_all(std::ifstream& input_fs, std::ofstream& outfupt_fs)
{
    // As everywhere, read/write checks
    std::uintmax_t blocks_count = 0;
    input_fs.read((char*)&blocks_count, sizeof(blocks_count));
    unsigned char* packed_buf = nullptr;
    std::uintmax_t packed_buf_size = 0;
    unsigned read_checksum = 0;
    for (std::uintmax_t i = 0; i < blocks_count; i++) {
        std::uintmax_t codon_count = 0, packed_size = 0;
        input_fs.read((char*)&codon_count, sizeof(codon_count));
        input_fs.read((char*)&packed_size, sizeof(packed_size));
        if (packed_size > packed_buf_size) {
            delete[] packed_buf;
            packed_buf_size = packed_size;
            packed_buf = new unsigned char[packed_buf_size];
        }
        input_fs.read((char*)packed_buf, packed_size);
        input_fs.read((char*)&read_checksum, sizeof(read_checksum));
        unsigned my_checksum = crc32(packed_buf, packed_size);
        if (read_checksum != my_checksum) {
            delete[] packed_buf;
            return ERR_CRC;
        }
        pos_t* compressed_buf = unpack(codon_count, packed_buf);
        std::string decompressed_data;
        int state = decompress(compressed_buf, codon_count, decompressed_data);
        delete[] compressed_buf;
        if (state != OK) {
            delete[] packed_buf;
            return state;
        }
        outfupt_fs.write(decompressed_data.c_str(), decompressed_data.size());
    }
    delete[] packed_buf;
    return OK;
}

namespace tld {
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

unsigned crc32(const unsigned char *data_buf, std::size_t data_size)
{
    static unsigned table[256] = {};
    static bool have_table = false;
    if (!have_table) {
        crc_generate_table(table);
        have_table = true;
    }
    unsigned crc = 0xffffffffu;
    unsigned char octet = 0;
    const unsigned char* q = data_buf + data_size;
    for (const unsigned char* p = data_buf; p < q; p++) {
        octet = *p;
        crc = (crc >> 8) ^ table[(crc & 0xffu) ^ octet];
    }
    return ~crc;
}

} // namespace tld
