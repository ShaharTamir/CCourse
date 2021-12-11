#include <stdio.h>
#include "test_input.h"

void PrintInputInstructions(int max_input_length)
{
    printf("To run the automatic tests run again with the names of files you wish to run.\n");
    printf("If the files are in a different folder - add path.\n");
    printf("If you wish to enter a manual test start the program without parameters\n");
    printf("and enter your string. The SW will check maximum %d chars from the string.\n", max_input_length);
    printf("The %d char and on will be ignored.\n\n", max_input_length + 1);
}

void HandleInput(int argc, char *argv[], InputTestFunc single_test_func)
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

