/* std */
#include <stdio.h> /* fopen, fclose */

/* sw */
#include "basic_defs.h"
#include "file_handler.h"
#include "pre_processor.h"
#include "assembler.h"

int main(int argc, char *argv[])
{
    FILE* input = NULL;
    char* file_name = NULL;
    int status = FALSE;

    if(argc > 1)
    {
        file_name = GetFileName(argv[1], STAGE_FIRST);
        input = OpenFile(file_name, "r");

        if(input)
        {
            status = RunPreProcessor(input, argv[1]);
            fclose(input);

            if(status)
            {
                input = fopen(file_name, "r");
                if(input)
                {
                    RunAssembler(input, argv[1]);
                    printf("success\n");
                    fclose(input);
                }
                else {}
                /* could not open the processed file (reach here is very bad..) */
            }
            else {}
            /* PreProcessor failed for some reason. */
        }
        else 
        {
        }
        /* could not open input file */
    }

    return 0;
}