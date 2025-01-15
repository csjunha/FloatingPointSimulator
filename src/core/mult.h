#ifndef __MULT_H__
#define __MULT_H__

#include <cstdint>

#include "./fp.h"
#include "./util.h"
#include "./assert.h"

FP mult(
    FP a,
    FP b,
    offset_t result_exp_size,
    offset_t result_man_size,
    offset_t result_padamt);

#endif // __MULT_H__
