#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <ctime>

#include "./core/fp.h"
#include "./core/adder_tree.h"
#include "./core/util.h"
#include "./config/config.h"

int main()
{
    std::srand(std::time(nullptr));

    FP *fps = (FP *)malloc(sizeof(FP) * VECTOR_SIZE);
    for (int i = 0; i < VECTOR_SIZE; i++)
    {
        float random_float = gen_random_float32();
        uint32_t data = *reinterpret_cast<uint32_t*>(&random_float);

        fps[i] = FP((uint64_t)data, 8, 23);
    }

    FP result = add_all(fps, VECTOR_SIZE);
    result.dump();
}
