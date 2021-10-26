#ifndef __TEST_INPUT_H__
#define __TEST_INPUT_H__

#include <stdio.h>

typedef void (*InputTestFunc)(FILE*);

void handleInput(int argc, char *argv[], InputTestFunc single_test_func);


#endif /* __TEST_INPUT_H__ */