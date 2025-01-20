#include <iostream>
#include <fstream>

#include <boost/program_options.hpp>

#include "./core/defaults.h"
#include "./core/file_writer.h"
#include "./core/util.h"

namespace po = boost::program_options;

void generate(
    std::string file,
    uint32_t num_vectors,
    uint32_t vector_size,
    uint8_t exp_width,
    uint8_t man_width,
    float from,
    float to)
{
    FileWriter writer(file);
    uint8_t size = 1 + exp_width + man_width;
    for (uint32_t i = 0; i < num_vectors; i++)
    {
        for (uint32_t j = 0; j < vector_size; j++)
        {
            float value = gen_random_float32(from, to);
            uint64_t data = convert_float_precision(value, exp_width, man_width);

            writer.write_as_binary(data, size, false);
        }

        if (i < num_vectors - 1)
            writer.write_line("");
    }
}

int main(int argc, char *argv[])
{
    po::options_description desc("Options");
    desc.add_options()
        ("help,h",                                                                                      "Print help message")
        ("output-files",            po::value<std::vector<std::string>>()->multitoken()->required(),    "Output filenames")
        ("num-vectors",             po::value<uint32_t>()->default_value(DEFAULT_NUM_VECTORS),          "Number of vectors to test")
        ("vector-size",             po::value<uint32_t>()->default_value(DEFAULT_VECTOR_SIZE),          "Size of the input vectors")
        ("exp-width",               po::value<uint32_t>()->default_value(DEFAULT_INPUT_EXP_WIDTH),      "Exponent width of the input vectors")
        ("man-width",               po::value<uint32_t>()->default_value(DEFAULT_INPUT_MAN_WIDTH),      "Mantissa width of the input vectors")
        ("range-from",              po::value<float>()->default_value(-1.0f),                           "Range from")
        ("range-to",                po::value<float>()->default_value(1.0f),                            "Range to");

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

    std::vector<std::string> output_files = vm["output-files"].as<std::vector<std::string>>();
    uint32_t num_vectors = vm["num-vectors"].as<uint32_t>();
    uint32_t vector_size = vm["vector-size"].as<uint32_t>();

    uint8_t exp_width = (uint8_t)vm["exp-width"].as<uint32_t>();
    uint8_t man_width = (uint8_t)vm["man-width"].as<uint32_t>();

    float from = vm["range-from"].as<float>();
    float to = vm["range-to"].as<float>();

    try {
        for (std::string file : output_files)
            generate(file, num_vectors, vector_size, exp_width, man_width, from, to);
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
}
