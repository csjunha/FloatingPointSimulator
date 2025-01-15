#ifndef __UTIL_H__
#define __UTIL_H__

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <random>

#include "assert.h"

uint64_t get_bitmask_64(uint32_t begin, uint32_t end);

float gen_random_float32();

uint8_t clog2(uint64_t num);

void dump_bits(uint64_t data, uint8_t size);

#endif // __UTIL_H__
