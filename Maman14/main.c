/* std */
#include <stdio.h> /* fopen, fclose */

/* sw */
#include "pre_processor.h"
/*#include "assembler.h"*/

int main(int argc, char *argv[])
{
    FILE* input = NULL;
    char* processed_input_name = NULL;
    FILE* processed_input = NULL;

    if(argc > 1)
    {
        input = fopen(argv[1], "r");

        if(input)
        {
            processed_input_name = RunPreProcessor(input, argv[1]);
            fclose(input);

            if(processed_input_name)
            {
                processed_input = fopen(processed_input_name, "r");
                if(processed_input)
                {
                    RunAssembler(processed_input);
                    fclose(processed_input);
                }
                else {}
                /* could not open the processed file (reach here is very bad..) */
            }
            else {}
            /* PreProcessor failed for some reason. */
        }
        else {}
        /* could not open input file */
    }

    return 0;
}