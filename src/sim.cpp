#include <fstream>
#include <iostream>
#include <ctime>
#include <cstdio>
#include <malloc.h>
#include <stdlib.h>
#include <unistd.h>

#include <boost/program_options.hpp>

#include "./core/fp.h"
#include "./core/adder_tree.h"
#include "./core/mult.h"
#include "./core/ipu.h"
#include "./core/accumulator.h"
#include "./core/util.h"

#include "./core/global_config.h"
#include "./core/defaults.h"

#include "./core/file_reader.h"
#include "./core/file_writer.h"

namespace po = boost::program_options;

FP **allocate_vectors()
{
    GlobalConfig &config = GlobalConfig::get_instance();

    uint32_t num_vectors = config.get_num_vectors();
    uint32_t vector_size = config.get_vector_size();

    FP **vectors = (FP **)malloc(sizeof(FP *) * num_vectors);
    for (uint32_t i = 0; i < num_vectors; i++)
        vectors[i] = (FP *)malloc(sizeof(FP) * vector_size);

    return vectors;
}

void free_vectors(FP **vectors)
{
    GlobalConfig &config = GlobalConfig::get_instance();

    uint32_t num_vectors = config.get_num_vectors();

    for (uint32_t i = 0; i < num_vectors; i++)
    {
        free(vectors[i]);
    }
    free(vectors);
}

FP **read_and_parse_input_file(std::string input_file)
{
    GlobalConfig &config = GlobalConfig::get_instance();

    uint32_t num_vectors = config.get_num_vectors();
    uint32_t vector_size = config.get_vector_size();

    uint32_t input_exp_width = config.get_input_exp_width();
    uint32_t input_man_width = config.get_input_man_width();
    uint32_t input_num_size = 1 + input_exp_width + input_man_width;

    FP **vectors = allocate_vectors();

    try
    {
        FileReader reader(input_file);

        std::string line;

        uint32_t remaining_lines = num_vectors;
        uint32_t expected_line_size = input_num_size * vector_size;

        while (reader.read_line(line) && remaining_lines > 0)
        {
            // Trim whitespaces
            size_t start = line.find_first_not_of(" \t\n\r");
            size_t end = line.find_last_not_of(" \t\n\r");

            if (start == std::string::npos)
                line.clear();
            else
                line = line.substr(start, end - start + 1);

            if (line.empty())
                continue;

            if (line.size() != expected_line_size)
                throw std::runtime_error("Invalid line size in the input file");

            uint32_t vector_idx = num_vectors - remaining_lines;

            for (uint32_t i = 0; i < vector_size; i++)
            {
                std::string num_str = line.substr(i * input_num_size, input_num_size);
                uint64_t num = convert_binary_string_to_uint64(num_str);

                vectors[vector_idx][i] = FP(num, input_exp_width, input_man_width);
            }

            remaining_lines--;
        }

        if (remaining_lines != 0)
            throw std::runtime_error("Insufficient number of lines in the input file");

        return vectors;
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
        free_vectors(vectors);
        return NULL;
    }
}

int main(int argc, char *argv[])
{
    GlobalConfig &config = GlobalConfig::get_instance();

    po::options_description desc("Options");
    desc.add_options()("help,h", "Print help message")("input-files", po::value<std::vector<std::string>>()->multitoken()->required(), "Input file paths (number of files should be exactly 2)")("output-file", po::value<std::string>()->required(), "Output filename")("num-vectors", po::value<uint32_t>()->default_value(DEFAULT_NUM_VECTORS), "Number of vectors to test")("vector-size", po::value<uint32_t>()->default_value(DEFAULT_VECTOR_SIZE), "Size of the input vectors")("add-pad-width", po::value<uint32_t>()->default_value(DEFAULT_ADD_PAD_WIDTH), "Padding width for addition")("adder-tree-pad-width", po::value<uint32_t>()->default_value(DEFAULT_ADDER_TREE_PAD_WIDTH), "Padding width for the adder tree")("input-exp-width", po::value<uint32_t>()->default_value(DEFAULT_INPUT_EXP_WIDTH), "Exponent width of the input vectors")("input-man-width", po::value<uint32_t>()->default_value(DEFAULT_INPUT_MAN_WIDTH), "Mantissa width of the input vectors")("result-exp-width", po::value<uint32_t>()->default_value(DEFAULT_RESULT_EXP_WIDTH), "Exponent width of the result")("result-man-width", po::value<uint32_t>()->default_value(DEFAULT_RESULT_MAN_WIDTH), "Mantissa width of the result");

    po::variables_map vm;

    try
    {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    }
    catch (const po::error &ex)
    {
        std::cerr << "Error: " << ex.what() << "\n\n";
        std::cerr << desc << std::endl;
        return 1;
    }

    if (vm.count("help"))
    {
        std::cout << desc << std::endl;
        return 0;
    }

    if (vm["input-files"].as<std::vector<std::string>>().size() != 2)
    {
        std::cerr << "Error: Number of input files should be exactly 2\n\n";
        std::cerr << desc << std::endl;
        return 1;
    }

    std::string input_operand_file_a = vm["input-files"].as<std::vector<std::string>>()[0];
    std::string input_operand_file_b = vm["input-files"].as<std::vector<std::string>>()[1];
    std::string output_file = vm["output-file"].as<std::string>();

    printf("Input Operand File A: %s\n", input_operand_file_a.c_str());
    printf("Input Operand File B: %s\n\n", input_operand_file_b.c_str());

    config.set_num_vectors(vm["num-vectors"].as<uint32_t>());
    config.set_vector_size(vm["vector-size"].as<uint32_t>());

    config.set_add_pad_width((offset_t)vm["add-pad-width"].as<uint32_t>());
    config.set_adder_tree_pad_width((offset_t)vm["adder-tree-pad-width"].as<uint32_t>());
    config.set_input_exp_width((offset_t)vm["input-exp-width"].as<uint32_t>());
    config.set_input_man_width((offset_t)vm["input-man-width"].as<uint32_t>());
    config.set_result_exp_width((offset_t)vm["result-exp-width"].as<uint32_t>());
    config.set_result_man_width((offset_t)vm["result-man-width"].as<uint32_t>());

    config.dump();

    std::cout << "\nReading and parsing input files...\n"
              << std::endl;

    FP **vectors_a = read_and_parse_input_file(input_operand_file_a);
    if (!vectors_a)
    {
        return 1;
    }

    FP **vectors_b = read_and_parse_input_file(input_operand_file_b);
    if (!vectors_b)
    {
        free_vectors(vectors_a);
        return 1;
    }

    std::cout << "\nProcessing inner product...\n"
              << std::endl;

    offset_t result_exp_size = config.get_result_exp_width();
    offset_t result_man_size = config.get_result_man_width();

    Accumulator accumulator(result_exp_size, result_man_size);
    FileWriter writer(output_file);

    for (uint32_t i = 0; i < config.get_num_vectors(); i++)
    {
        FP result = inner_product(
            vectors_a[i],
            vectors_b[i],
            result_exp_size,
            result_man_size);

        accumulator.accumulate(result);

        FP acc = accumulator.get_result();
        writer.write_as_binary(acc.encode(), acc.size);
    }

    std::cout << "\nDone!\n" << std::endl;
}

// #define PATH_MAX 1024

// int main()
// {
//     std::srand(std::time(nullptr));

//     FP *vecA = (FP *)malloc(sizeof(FP) * VECTOR_SIZE);
//     FP *vecB = (FP *)malloc(sizeof(FP) * VECTOR_SIZE);

//     float acc = 0;

//     for (int i = 0; i < VECTOR_SIZE; i++)
//     {
//         float random_floatA = gen_random_float32(-0.0001f, 0.0001f);
//         float random_floatB = gen_random_float32(-0.0001f, 0.0001f);
//         acc += random_floatA * random_floatB;

//         uint32_t dataA = *reinterpret_cast<uint32_t *>(&random_floatA);
//         uint32_t dataB = *reinterpret_cast<uint32_t *>(&random_floatB);
//         uint16_t dataA_fp16 = fp32_to_fp16(dataA);
//         uint16_t dataB_fp16 = fp32_to_fp16(dataB);

//         dump_bits_hex("dataA", dataA, 32);
//         dump_bits_hex("dataA_fp16", dataA_fp16, 16);

//         vecA[i] = FP((uint64_t)dataA_fp16, INPUT_EXP_WIDTH, INPUT_MAN_WIDTH);
//         vecB[i] = FP((uint64_t)dataB_fp16, INPUT_EXP_WIDTH, INPUT_MAN_WIDTH);
//     }

//     FP result = inner_product(
//         vecA,
//         vecB,
//         VECTOR_SIZE,
//         RESULT_EXP_WIDTH,
//         RESULT_MAN_WIDTH);

//     free(vecA);
//     free(vecB);

//     result.dump("Inner Product Result");
//     printf("C++ FP32 Result: %.16f\n", acc);
// }
