#include "./util.h"

// get_bitmask_64 returns a bitmask of 64 bits with 1s from high to low
uint64_t get_bitmask_64(uint8_t high, uint8_t low)
{
    ASSERT(high >= low);

    uint8_t size = high - low + 1;
    return (((uint64_t)1 << size) - 1) << low;
}

float gen_random_float32(float range_from, float range_to) {
    std::random_device rd;
    std::mt19937 generator(rd());

    std::uniform_real_distribution<float> distribution(range_from, range_to);
    return distribution(generator);
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

void dump_bits(const char* tag, uint64_t data, uint8_t size)
{
    printf("%s: ", tag);
    for (int i = size - 1; i >= 0; i--)
    {
        printf("%d", (data & ((uint64_t)1 << i)) ? 1 : 0);
    }
    printf("\n");
}