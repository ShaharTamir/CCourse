#include <stdio.h>
#include "test_input.h"

void handleInput(int argc, char *argv[], InputTestFunc single_test_func)
{
    FILE *file = stdin;

    if(argc > 1)
    {
        int i = 1;
        int testNumber = 1;    

        for(i = 1; i < argc; ++i, ++testNumber)
        {
            file = fopen(argv[i], "r");
            if(file)
            {
                printf("test number %d:\n", testNumber);
                single_test_func(file);
                fclose(file);
            }
            else
            {
                printf("could not open file %s\n", argv[i]);
            }
        }
    }
    else
    {
        single_test_func(file);
    }
}