#include "./global_config.h"

GlobalConfig &GlobalConfig::get_instance()
{
    static GlobalConfig instance;
    return instance;
}

GlobalConfig::GlobalConfig() : adder_tree_pad_width(DEFAULT_ADDER_TREE_PAD_WIDTH),
                               add_pad_width(DEFAULT_ADD_PAD_WIDTH),
                               input_exp_width(DEFAULT_INPUT_EXP_WIDTH),
                               input_man_width(DEFAULT_INPUT_MAN_WIDTH),
                               result_exp_width(DEFAULT_RESULT_EXP_WIDTH),
                               result_man_width(DEFAULT_RESULT_MAN_WIDTH)
{
}

GlobalConfig::~GlobalConfig() {}

// Getters
uint32_t GlobalConfig::get_num_vectors() const
{
    return this->num_vectors;
}

uint32_t GlobalConfig::get_vector_size() const
{
    return this->vector_size;
}

offset_t GlobalConfig::get_adder_tree_pad_width() const
{
    return this->adder_tree_pad_width;
}

offset_t GlobalConfig::get_add_pad_width() const
{
    return this->add_pad_width;
}

offset_t GlobalConfig::get_input_exp_width() const
{
    return this->input_exp_width;
}

offset_t GlobalConfig::get_input_man_width() const
{
    return this->input_man_width;
}

offset_t GlobalConfig::get_result_exp_width() const
{
    return this->result_exp_width;
}

offset_t GlobalConfig::get_result_man_width() const
{
    return this->result_man_width;
}

// Setters
void GlobalConfig::set_num_vectors(uint32_t num_vectors)
{
    this->num_vectors = num_vectors;
}

void GlobalConfig::set_vector_size(uint32_t vector_size)
{
    this->vector_size = vector_size;
}

void GlobalConfig::set_adder_tree_pad_width(offset_t adder_tree_pad_width)
{
    this->adder_tree_pad_width = adder_tree_pad_width;
}

void GlobalConfig::set_add_pad_width(offset_t add_pad_width)
{
    this->add_pad_width = add_pad_width;
}

void GlobalConfig::set_input_exp_width(offset_t input_exp_width)
{
    this->input_exp_width = input_exp_width;
}

void GlobalConfig::set_input_man_width(offset_t input_man_width)
{
    this->input_man_width = input_man_width;
}

void GlobalConfig::set_result_exp_width(offset_t result_exp_width)
{
    this->result_exp_width = result_exp_width;
}

void GlobalConfig::set_result_man_width(offset_t result_man_width)
{
    this->result_man_width = result_man_width;
}

void GlobalConfig::dump()
{
    std::cout << "======= Global Configurations =======" << std::endl;
    printf("Number of Vectors: %d\n", this->num_vectors);
    printf("Vector Size: %d\n", this->vector_size);
    printf("Adder Tree Pad Width: %d\n", this->adder_tree_pad_width);
    printf("Add Pad Width: %d\n", this->add_pad_width);
    printf("Input Exponent Width: %d\n", this->input_exp_width);
    printf("Input Mantissa Width: %d\n", this->input_man_width);
    printf("Result Exponent Width: %d\n", this->result_exp_width);
    printf("Result Mantissa Width: %d\n", this->result_man_width);
    std::cout << "=====================================" << std::endl;
}
