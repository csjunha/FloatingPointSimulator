#include "./util.h"

const char *hex_table = "0123456789ABCDEF";

// get_bitmask_64 returns a bitmask of 64 bits with 1s from high to low
uint64_t get_bitmask_64(uint8_t high, uint8_t low)
{
    ASSERT(high >= low);

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

uint16_t fp32_to_fp16(uint32_t data)
{
    bool sign = (data & ((uint32_t)1 << 31)) != 0;
    int16_t exp32_raw = (data >> 23) & 0xFF;

    uint32_t uman32 = (data & 0x7FFFFF) | (exp32_raw == 0 ? 0 : (uint32_t)1 << 23);
    int16_t exp32 = exp32_raw == 0 ? 1 : exp32_raw;

    if (uman32 == 0)
    {
        return 0 | (sign << 15);
    }

    int16_t bias_diff = 127 - 15;
    int16_t exp16 = exp32 - bias_diff;

    if (exp16 >= 31)
    {
        return ((uint16_t)sign << 15) | ((uint16_t)31 << 10);
    }
    else if (exp16 <= 0)
    {
        int16_t shamt = 1 - exp16;
        uint32_t man32_shifted = uman32 >> shamt;
        uint16_t man16 = (man32_shifted >> 13) & 0x3FF; // Truncate
        return ((uint16_t)sign) << 15 | man16;
    }
    else
    {
        uint16_t man16 = (uman32 >> 13) & 0x3FF; // Truncate
        return ((uint16_t)sign) << 15 | ((uint16_t)exp16) << 10 | man16;
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
    uint8_t count = 0;
    while (num)
    {
        num >>= 1;
        count++;
    }
    return count;
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
