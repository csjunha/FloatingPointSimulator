#ifndef __ACCUMULATOR_H__
#define __ACCUMULATOR_H__

#include <stdint.h>

#include "./types.h"
#include "./fp.h"

class Accumulator
{
public:
    Accumulator(offset_t exp_size, offset_t man_size);
    ~Accumulator();

    void clear();
    void accumulate(const FP &input);

    FP get_result();

private:
    FP *acc;

    offset_t padamt;
};

#endif // __ACCUMULATOR_H__
