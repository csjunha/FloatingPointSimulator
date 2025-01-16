#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <ctime>

#include "./core/fp.h"
#include "./core/adder_tree.h"
#include "./core/mult.h"
#include "./core/ipu.h"
#include "./core/util.h"
#include "./config/config.h"

int main()
{
    std::srand(std::time(nullptr));

    FP *vecA = (FP *)malloc(sizeof(FP) * VECTOR_SIZE);
    FP *vecB = (FP *)malloc(sizeof(FP) * VECTOR_SIZE);

    float acc = 0;

    for (int i = 0; i < VECTOR_SIZE; i++)
    {
        float random_floatA = gen_random_float32(-0.0001f, 0.0001f);
        float random_floatB = gen_random_float32(-0.0001f, 0.0001f);
        acc += random_floatA * random_floatB;

        uint32_t dataA = *reinterpret_cast<uint32_t *>(&random_floatA);
        uint32_t dataB = *reinterpret_cast<uint32_t *>(&random_floatB);
        uint16_t dataA_fp16 = fp32_to_fp16(dataA);
        uint16_t dataB_fp16 = fp32_to_fp16(dataB);

        dump_bits_hex("dataA", dataA, 32);
        dump_bits_hex("dataA_fp16", dataA_fp16, 16);

        vecA[i] = FP((uint64_t)dataA_fp16, INPUT_EXP_WIDTH, INPUT_MAN_WIDTH);
        vecB[i] = FP((uint64_t)dataB_fp16, INPUT_EXP_WIDTH, INPUT_MAN_WIDTH);
    }

    FP result = inner_product(
        vecA,
        vecB,
        VECTOR_SIZE,
        RESULT_EXP_WIDTH,
        RESULT_MAN_WIDTH);

    free(vecA);
    free(vecB);

    result.dump("Inner Product Result");
    printf("C++ FP32 Result: %.16f\n", acc);
}
