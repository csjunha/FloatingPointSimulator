//////////////////////////////////////////////////////////////////////////
//  Bit-accurate Floating Point Simulator Configurations
//////////////////////////////////////////////////////////////////////////

#ifndef __CONFIG_H__
#define __CONFIG_H__

#define INPUT_EXP_WIDTH 5
#define INPUT_MAN_WIDTH 10

#define RESULT_EXP_WIDTH 8
#define RESULT_MAN_WIDTH 23

#define ADDER_TREE_PAD_WIDTH 4

#define ADDER_TREE_MAN_WIDTH ADDER_TREE_PAD_WIDTH + RESULT_MAN_WIDTH

#define VECTOR_SIZE 64

#endif // __CONFIG_H__