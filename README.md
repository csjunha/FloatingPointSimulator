# Bit-accurate FP inner product simulator

Bit-accurate floating-point inner product simulator for testing and verification of floating-point inner product implementations.

## Constraints

- Only the same precision is supported for both operands.
- Currently, double precision is not supported.

## Prerequisites

- **Boost C++ Library**: Make sure the path to the library is correctly set in the Makefile.

## Build

To build the simulator, run the following command at `src/`:

```bash
make
```

## Run

### Run simulator

To run the simulator, execute the following command at root:

```bash
./src/bin/sim --input-files <input_files> --output-file <output_file> --num-vectors <num_vectors> --vector-size <vector_size> --input-exp-width <input_exp_width> --input-man-width <input_man_width> --result-exp-width <result_exp_width> --result-man-width <result_man_width> --add-pad-width <add_pad_width> --adder-tree-pad-width <adder_tree_pad_width>
```

- `<input_files>`: Whitespace-separated list of input file paths. (e.g., `input1.dat input2.dat`)
- `<output_file>`: Output file paths.

### Run generator

To generate random input files, execute the following command at root:

```bash
./src/bin/gen --output-files <output_files> --num-vectors <num_vectors> --vector-size <vector_size> --exp-width <exp_width> --man-width <man_width> --range-from <range_from> --range-to <range_to>
```

- `<output_files>`: Whitespace-separated list of output file paths. (e.g., `input1.dat input2.dat`)
- `<num_vectors>`: Number of vectors.
- `<vector_size>`: Size of each vector.

## Example

```bash
./src/bin/gen --output-files ../data/operand_a.dat ../data/operand_b.dat --range-from "-0.0001" --range-to "0.0001"
./src/bin/sim --input-files ../data/operand_a.dat ../data/operand_b.dat --output-file ../data/result.dat
```
