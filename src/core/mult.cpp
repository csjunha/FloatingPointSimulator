#include "./mult.h"

FP mult(
    FP a,
    FP b,
    uint8_t result_exp_size,
    uint8_t result_man_size,
    uint8_t result_padamt)
{
    ASSERT(a.size == b.size);
    ASSERT(a.exp_size == b.exp_size);
    ASSERT(a.man_size == b.man_size);
    ASSERT(a.int_offset == b.int_offset);
    ASSERT(a.frac_offset == b.frac_offset);

    uint32_t operand_bias = (1 << (a.exp_size - 1)) - 1;

    uint32_t result_max_exp = (1 << result_exp_size) - 1;
    uint32_t result_bias = (1 << (result_exp_size - 1)) - 1;

    uint8_t operand_uman_high = a.int_offset - 1;
    uint8_t operand_uint_bitwidth = a.int_offset - a.frac_offset - 1;
    uint8_t operand_frac_bitwidth = a.frac_offset + 1;

    uint8_t mult_result_uint_bitwidth = operand_uint_bitwidth * 2;
    uint8_t mult_result_frac_bitwidth = operand_frac_bitwidth * 2;

    uint8_t uman_mult_result_bitwidth = mult_result_uint_bitwidth + mult_result_frac_bitwidth;
    uint8_t result_frac_offset = result_man_size - 1 + result_padamt;
    uint8_t result_uman_bitwidth = result_frac_offset + mult_result_uint_bitwidth + 1;

    if (a.man == 0 || b.man == 0)
    {
        return FP(
            result_exp_size,
            result_man_size,
            0,
            0,
            result_uman_bitwidth,
            result_frac_offset);
    }

    bool sign = a.sign ^ b.sign;
    uint64_t uman_a = a.get_uman();
    uint64_t uman_b = b.get_uman();

    uint8_t lz_a = get_leading_zero_count(uman_a, operand_uman_high, 0);
    uint8_t lz_b = get_leading_zero_count(uman_b, operand_uman_high, 0);

    uint8_t total_borrowed = lz_a + lz_b;

    uint64_t uman_a_shifted = uman_a << lz_a;
    uint64_t uman_b_shifted = uman_b << lz_b;
    uint64_t uman_mult = uman_a_shifted * uman_b_shifted;

    int32_t result_exp_raw = a.exp + b.exp + result_bias - 2 * operand_bias - total_borrowed;
    uint32_t result_exp;

    if (result_exp_raw < 0)
    {
        result_exp = 0;
        uman_mult >>= (-result_exp_raw);
    }
    else if ((uint32_t)result_exp_raw > result_max_exp)
    {
        // Infinity
        return FP(
            result_exp_size,
            result_man_size,
            0,
            result_max_exp,
            result_uman_bitwidth,
            result_frac_offset);
    }
    else
    {
        result_exp = (uint32_t)result_exp_raw;
    }

    // Size adjustment of uman_mult
    uint64_t uman_mult_adjusted;
    if (uman_mult_result_bitwidth < result_uman_bitwidth)
    {
        // Pad with zeros for the remaining bits
        uint8_t padamt = result_uman_bitwidth - uman_mult_result_bitwidth;
        uman_mult_adjusted = uman_mult << padamt;
    }
    else
    {
        // Truncate the result
        uint8_t truncate_high = uman_mult_result_bitwidth - 1;
        uint8_t truncate_low = truncate_high - result_uman_bitwidth + 1;

        uint64_t mask = get_bitmask_64(truncate_high, truncate_low);
        uman_mult_adjusted = (uman_mult & mask) >> truncate_low;
    }

    int64_t sman_mult = sign ? -(int64_t)uman_mult_adjusted : (int64_t)uman_mult_adjusted;

    return FP(
        result_exp_size,
        result_man_size,
        (uint64_t)sman_mult,
        result_exp,
        result_uman_bitwidth,
        result_frac_offset);
}
