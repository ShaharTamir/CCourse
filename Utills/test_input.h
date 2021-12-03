#ifndef __TEST_INPUT_H__
#define __TEST_INPUT_H__

#include <stdio.h> /* FILE type */

/* Template function to handle an input stream (file or stdin) */
typedef void (*InputTestFunc)(FILE*);

/**
 * PrintInputInstructions - 
 * A template for user instructions how to use the program inputs.
 * 
 * max_input_length: max length for a specific program / test.
 * 
 * return: NONE
*/
void PrintInputInstructions(int max_input_length);


/**
 * HandleInput -
 * Run the program (using single test function) with all given inputs.
 * Could be several files or stdin
 * 
 * argc: main argc.
 * argv: main argv.
 * single_test_function: As defined in the template a function pointer to handle 
 *  a single input on the program.
 * 
 * return: NONE
*/
void HandleInput(int argc, char *argv[], InputTestFunc single_test_func);


#endif /* __TEST_INPUT_H__ */