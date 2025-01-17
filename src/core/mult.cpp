#include "./mult.h"

FP mult(
    FP a,
    FP b,
    offset_t result_exp_size,
    offset_t result_man_size,
    offset_t result_padamt)
{
    assert(a.is_compatible_with(b));

    exp_t operand_bias = (1 << (a.exp_size - 1)) - 1;

    exp_t result_max_exp = (1 << result_exp_size) - 1;
    exp_t result_bias = (1 << (result_exp_size - 1)) - 1;

    offset_t operand_uman_high = a.int_offset - 1;
    offset_t operand_uint_bitwidth = a.int_offset - a.frac_offset - 1;
    offset_t operand_frac_bitwidth = a.frac_offset + 1;

    offset_t mult_result_uint_bitwidth = operand_uint_bitwidth * 2;
    offset_t mult_result_frac_bitwidth = operand_frac_bitwidth * 2;

    offset_t uman_mult_result_bitwidth = mult_result_uint_bitwidth + mult_result_frac_bitwidth;
    offset_t result_frac_offset = result_man_size - 1 + result_padamt;
    offset_t result_uman_bitwidth = result_frac_offset + mult_result_uint_bitwidth + 1;

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
    uman_t uman_a = a.get_uman();
    uman_t uman_b = b.get_uman();

    offset_t lz_a = get_leading_zero_count(uman_a, operand_uman_high, 0);
    offset_t lz_b = get_leading_zero_count(uman_b, operand_uman_high, 0);

    offset_t total_borrowed = lz_a + lz_b;

    uman_t uman_a_shifted = uman_a << lz_a;
    uman_t uman_b_shifted = uman_b << lz_b;
    uman_t uman_mult = uman_a_shifted * uman_b_shifted;

    int32_t result_exp_raw = a.exp + b.exp + result_bias - 2 * operand_bias - total_borrowed;
    exp_t result_exp;

    if (result_exp_raw < 0)
    {
        result_exp = 0;
        uman_mult >>= (-result_exp_raw);
    }
    else if ((exp_t)result_exp_raw > result_max_exp)
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
        result_exp = (exp_t)result_exp_raw;
    }

    // Size adjustment of uman_mult
    uman_t uman_mult_adjusted;
    if (uman_mult_result_bitwidth < result_uman_bitwidth)
    {
        // Pad with zeros for the remaining bits
        offset_t padamt = result_uman_bitwidth - uman_mult_result_bitwidth;
        uman_mult_adjusted = uman_mult << padamt;
    }
    else
    {
        // Truncate the result
        offset_t truncate_high = uman_mult_result_bitwidth - 1;
        offset_t truncate_low = truncate_high - result_uman_bitwidth + 1;

        uman_t mask = get_bitmask_64(truncate_high, truncate_low);
        uman_mult_adjusted = (uman_mult & mask) >> truncate_low;
    }

    sman_t sman_mult = sign ? -(sman_t)uman_mult_adjusted : (sman_t)uman_mult_adjusted;

    return FP(
        result_exp_size,
        result_man_size,
        sman_mult,
        result_exp,
        result_uman_bitwidth,
        result_frac_offset);
}
