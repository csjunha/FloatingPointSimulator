#ifndef __IPU_H__
#define __IPU_H__

#include "./fp.h"
#include "./mult.h"
#include "./adder_tree.h"

FP inner_product(
    FP *vector_a,
    FP *vector_b,
    uint32_t vector_size,
    offset_t result_exp_size,
    offset_t result_man_width);

#endif // __IPU_H__
