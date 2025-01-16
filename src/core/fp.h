#ifndef __FP_H__
#define __FP_H__

#include <cstdint>
#include <cmath>

#include "assert.h"
#include "../config/config.h"

#define INF(size, sign, man_size, exp_size) \
    ((uint64_t)(sign) << (size - 1)) |      \
        ((uint64_t)((1 << exp_size) - 1) << (man_size))

typedef uint8_t offset_t;
typedef uint32_t exp_t;
typedef int64_t sman_t;
typedef uint64_t uman_t;

class FP
{
public:
    offset_t size;
    offset_t exp_size;
    offset_t man_size;

    bool sign;
    exp_t exp;

    sman_t man;
    offset_t int_offset;  // FP[int_offset:frac_offset) is the integer part
    offset_t frac_offset; // FP[frac_offset:0] is the fractional part

    FP(const FP &other);
    FP(uint64_t data, offset_t exp_size, offset_t man_size);

    // Internal constructor
    FP(
        offset_t exp_size,
        offset_t man_size,
        sman_t sman,
        exp_t exp,
        offset_t int_offset,
        offset_t frac_offset);

    uman_t get_uman();
    bool is_compatible_with(const FP &other) const;
    void pad(offset_t padamt);
    void normalize();
    void round();
    uint64_t encode();

    void dump(const char *tag);

    float to_float();

    // Operators
    FP& operator=(const FP &other);
    FP operator+(const FP &other) const;

private:
    exp_t bias;
    exp_t exp_max;

    offset_t get_padbits_width();
    bool should_round_up();
    offset_t get_upnorm_shamt();
    offset_t get_downnorm_shamt();
    bool is_sman_out_of_range();

    inline bool is_normalized();
    inline bool is_rounded();
    inline bool is_normalized_and_rounded();

    inline void normalize_offsets();

    void dump_mantissa();
};

#endif // __FP_H__
