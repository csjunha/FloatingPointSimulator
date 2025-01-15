#include "../config/config.h"
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

FP add_all(FP *fps, uint32_t vector_size)
{
    // Assertion
    for (uint32_t i = 1; i < vector_size; i++)
    {
        ASSERT(fps[i].size == fps[0].size);
        ASSERT(fps[i].man_size == fps[0].man_size);
        ASSERT(fps[i].exp_size == fps[0].exp_size);
        ASSERT(fps[i].int_offset == fps[0].int_offset);
        ASSERT(fps[i].frac_offset == fps[0].frac_offset);
    }

    int64_t sman_acc = 0;
    uint32_t max_exp = find_max_exp(fps, vector_size);

    uint8_t vector_clog2 = clog2(vector_size);
    uint8_t result_int_offset = fps[0].int_offset + ADDER_TREE_PAD_WIDTH + vector_clog2;
    uint8_t result_frac_offset = fps[0].frac_offset + ADDER_TREE_PAD_WIDTH;

    // 1. Pre-normalize
    for (uint32_t i = 0; i < vector_size; i++)
    {
        FP current = fps[i];
        current.pad(ADDER_TREE_PAD_WIDTH);

        uint32_t shamt = max_exp - current.exp;
        int64_t sman = current.man >> shamt;
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

    result.dump();

    result.normalize();
    result.round();

    return result;
}
