#ifndef __GLOBAL_CONFIG_H__
#define __GLOBAL_CONFIG_H__

#include <iostream>
#include <memory>
#include <mutex>

#include "./types.h"
#include "./defaults.h"

class GlobalConfig
{
public:
    // Static method to access the singleton instance
    static GlobalConfig &get_instance();

    // Operators
    GlobalConfig(const GlobalConfig &) = delete;
    GlobalConfig &operator=(const GlobalConfig &) = delete;

    // Getters
    uint32_t get_num_vectors() const;
    uint32_t get_vector_size() const;
    offset_t get_adder_tree_pad_width() const;
    offset_t get_add_pad_width() const;
    offset_t get_input_exp_width() const;
    offset_t get_input_man_width() const;
    offset_t get_result_exp_width() const;
    offset_t get_result_man_width() const;

    // Setters
    void set_num_vectors(uint32_t num_vectors);
    void set_vector_size(uint32_t vector_size);
    void set_adder_tree_pad_width(offset_t adder_tree_pad_width);
    void set_add_pad_width(offset_t add_pad_width);
    void set_input_exp_width(offset_t input_exp_width);
    void set_input_man_width(offset_t input_man_width);
    void set_result_exp_width(offset_t result_exp_width);
    void set_result_man_width(offset_t result_man_width);

    // Dump
    void dump();

private:
    GlobalConfig();
    ~GlobalConfig();

    uint32_t num_vectors;
    uint32_t vector_size;

    offset_t adder_tree_pad_width;
    offset_t add_pad_width;
    offset_t input_exp_width;
    offset_t input_man_width;
    offset_t result_exp_width;
    offset_t result_man_width;
};

#endif // __GLOBAL_CONFIG_H__
