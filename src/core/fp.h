#ifndef __FP_H__
#define __FP_H__

#include <cstdint>

#define INF(size, sign, man_size, exp_size)      \
    ((uint64_t)(sign) << (size - 1)) | \
        ((uint64_t)((1 << exp_size) - 1) << (man_size))

class FP
{
public:
    uint16_t size;
    uint8_t exp_size;
    uint8_t man_size;

    bool sign;
    uint32_t exp;

    int64_t man;
    uint8_t int_offset;  // FP[int_offset:frac_offset) is the integer part
    uint8_t frac_offset; // FP[frac_offset:0] is the fractional part

    FP(uint64_t data, uint8_t exp_size, uint8_t man_size);

    // Internal constructor
    FP(
        uint8_t exp_size,
        uint8_t man_size,
        uint64_t sman,
        uint32_t exp,
        uint8_t int_offset,
        uint8_t frac_offset);

    void dump();

    void pad(uint8_t padamt);
    void normalize();
    void round();
    uint64_t encode();

private:
    uint32_t bias;
    uint32_t exp_max;

    uint64_t get_uman();
    uint8_t get_padbits_width();

    bool should_round_up();
    uint32_t get_upnorm_shamt();
    uint32_t get_downnorm_shamt();
    bool is_sman_out_of_range();

    inline bool is_normalized();
    inline void normalize_offsets();

    void dump_mantissa();
};

#endif // __FP_H__
