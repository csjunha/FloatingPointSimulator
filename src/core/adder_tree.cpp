#include "./adder_tree.h"

uint32_t find_max_exp(FP *fps, uint32_t vector_size)
{
    uint32_t max = 0;
    for (uint32_t i = 0; i < vector_size; i++)
    {
        if (fps[i].exp > max)
            max = fps[i].exp;
    }
    return max;
}

FP adder_tree(FP *fps, uint32_t vector_size)
{
    // Assertion
    for (uint32_t i = 1; i < vector_size; i++)
        assert(fps[0].is_compatible_with(fps[i]));

    sman_t sman_acc = 0;
    exp_t max_exp = find_max_exp(fps, vector_size);

    offset_t vector_size_clog2 = clog2(vector_size);
    offset_t result_int_offset = fps[0].int_offset + vector_size_clog2;
    offset_t result_frac_offset = fps[0].frac_offset;

    // 1. Pre-normalize
    for (uint32_t i = 0; i < vector_size; i++)
    {
        FP current = fps[i];
        exp_t shamt = max_exp - current.exp;
        sman_t sman = current.man >> shamt;
        sman_acc += sman;
    }

    // 2. Instantiation & Post-normalize
    FP result(
        fps[0].exp_size,
        fps[0].man_size,
        sman_acc,
        max_exp,
        result_int_offset,
        result_frac_offset);

    result.normalize();
    result.round();

    return result;
}
