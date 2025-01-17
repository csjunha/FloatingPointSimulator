#include "./util.h"

const char *hex_table = "0123456789ABCDEF";

// get_bitmask_64 returns a bitmask of 64 bits with 1s from high to low
uint64_t get_bitmask_64(uint8_t high, uint8_t low)
{
    assert(high >= low);

    uint8_t size = high - low + 1;
    return (((uint64_t)1 << size) - 1) << low;
}

float gen_random_float32(float range_from, float range_to)
{
    std::random_device rd;
    std::mt19937 generator(rd());

    std::uniform_real_distribution<float> distribution(range_from, range_to);
    return distribution(generator);
}

uint64_t convert_float_precision(float src, uint8_t dest_exp_size, uint8_t dest_man_size)
{
    uint8_t dest_size = 1 + dest_exp_size + dest_man_size;
    assert(dest_size <= 64);

    int32_t dest_exp_bias = (1 << (dest_exp_size - 1)) - 1;
    int32_t dest_exp_max = (1 << dest_exp_size) - 1;

    uint32_t data = *reinterpret_cast<uint32_t *>(&src);

    bool sign = (data & ((uint32_t)1 << 31)) != 0;

    int32_t src_exp_bias = 127;
    int32_t src_exp_bits = (data >> 23) & 0xFF;
    int32_t src_exp = src_exp_bits == 0 ? 1 : src_exp_bits;
    uint64_t src_uman = (data & 0x7FFFFF) | (src_exp_bits == 0 ? 0 : (uint64_t)1 << 23);

    if (src_uman == 0)
    {
        return ((uint64_t)sign << (dest_size - 1));
    }

    int32_t bias_diff = src_exp_bias - dest_exp_bias;
    int32_t desired_dest_exp = src_exp - bias_diff;

    if (desired_dest_exp >= dest_exp_max)
    {
        return ((uint64_t)sign << (dest_size - 1)) | ((((uint64_t)1 << dest_exp_size) - 1) << dest_man_size);
    }
    else if (desired_dest_exp <= 0)
    {
        uint8_t shamt = 1 - desired_dest_exp;

        if (dest_man_size >= 23)
        {
            uint64_t discarded_bits = src_uman & get_bitmask_64(shamt - 1, 0);
            uint64_t src_uman_shifted = src_uman >> shamt;
            uint8_t remaining_size = dest_man_size - 23;

            uint64_t result = ((uint64_t)sign << (dest_size - 1)) | ((src_uman_shifted & get_bitmask_64(22, 0)) << remaining_size);

            if (remaining_size == 0)
                return result;

            if (shamt >= remaining_size)
                return result | ((discarded_bits & get_bitmask_64(shamt - 1, shamt - remaining_size)) >> (shamt - remaining_size));
            else
                return result | (discarded_bits << (remaining_size - shamt));
        }
        else
        {
            uint64_t src_uman_shifted = src_uman >> shamt;
            return ((uint64_t)sign << (dest_size - 1)) |
                   (src_uman_shifted & get_bitmask_64(22, 22 - dest_man_size + 1)) >> (22 - dest_man_size + 1);
        }
    }
    else
    {
        uint64_t man = 0;
        if (dest_man_size >= 23)
        {
            man = (src_uman << (dest_man_size - 23)) & get_bitmask_64(dest_man_size - 1, 0); // Pad
        }
        else
        {
            man = (src_uman & get_bitmask_64(22, 22 - dest_man_size + 1)) >> (22 - dest_man_size + 1); // Truncate
        }
        return ((uint64_t)sign << (dest_size - 1)) | (((uint64_t)desired_dest_exp) << dest_man_size) | man;
    }
}

uint8_t get_leading_zero_count(uint64_t data, uint8_t start, uint8_t end)
{
    uint8_t lz = 0;
    for (int i = start; i >= end; --i)
    {
        uint64_t mask = (uint64_t)1 << i;
        if (data & mask)
        {
            lz = start - (uint8_t)i;
            break;
        }
    }

    return lz;
}

uint8_t clog2(uint64_t num)
{
    if (num <= 1)
        return 0;

    --num;
    uint8_t count = 0;
    while (num > 0)
    {
        num >>= 1;
        count++;
    }
    return count;
}

uint64_t convert_binary_string_to_uint64(std::string str)
{
    if (str.size() > 64)
    {
        throw std::runtime_error("String size exceeds 64 bits");
    }

    uint64_t data = 0;
    for (size_t i = 0; i < str.size(); i++)
    {
        if (str[i] != '0' && str[i] != '1')
        {
            throw std::runtime_error("Invalid character found in binary string");
        }
        data |= (str[i] == '1') ? ((uint64_t)1 << (str.size() - 1 - i)) : 0;
    }

    return data;
}

void dump_bits(const char *tag, uint64_t data, uint8_t size)
{
    printf("%s: ", tag);
    for (int i = size - 1; i >= 0; i--)
    {
        printf("%d", (data & ((uint64_t)1 << i)) ? 1 : 0);
    }
    printf("\n");
}

void dump_bits_hex(const char *tag, uint64_t data, uint8_t size)
{
    uint8_t ceil = ((size + 3) / 4) * 4;

    printf("%s: ", tag);
    for (int i = ceil; i > 0; i -= 4)
    {
        uint8_t hex = (data >> (i - 4)) & 0xF;
        printf("%c", hex_table[hex]);
    }
    printf("\n");
}
