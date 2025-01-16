#ifndef __UTIL_H__
#define __UTIL_H__

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <random>

#include "assert.h"

uint64_t get_bitmask_64(uint8_t begin, uint8_t end);

float gen_random_float32(float range_from, float range_to);

uint16_t fp32_to_fp16(uint32_t data);

uint8_t get_leading_zero_count(uint64_t data, uint8_t start, uint8_t end);

uint8_t clog2(uint64_t num);

void dump_bits(const char* tag, uint64_t data, uint8_t size);
void dump_bits_hex(const char* tag, uint64_t data, uint8_t size);

#endif // __UTIL_H__
