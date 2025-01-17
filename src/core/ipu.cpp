#include "./ipu.h"

FP inner_product(
    FP *vector_a,
    FP *vector_b,
    offset_t result_exp_size,
    offset_t result_man_width)
{
    GlobalConfig &config = GlobalConfig::get_instance();
    uint32_t vector_size = config.get_vector_size();

    FP *products = (FP *)malloc(sizeof(FP) * vector_size);

    for (uint32_t i = 0; i < vector_size; i++)
    {
        products[i] = mult(
            vector_a[i],
            vector_b[i],
            result_exp_size,
            result_man_width,
            config.get_adder_tree_pad_width());
    }

    FP result = adder_tree(products, vector_size);

    free(products);

    return result;
}
