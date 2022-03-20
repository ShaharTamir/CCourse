/* std */
#include <stdio.h> /* fopen, fclose */
#include <stdlib.h>

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
    int i = 0;

    if(argc <= 1)
    {
        printf("%sassembler: %sfatal error: %sno input files\n", CLR_BOLD, CLR_RED, CLR_RESET);
        printf("compilation terminated\n");
    }

    InitGlobals();
    
    for(i = 1; i < argc; ++i)
    {
        file_name = FileHandlerGetFileName(argv[i], STAGE_FIRST);
        input = FileHandlerOpenFile(file_name, "r");
        
        free(file_name);
        file_name = NULL;

        if(input)
        {
            status = RunPreProcessor(input, argv[i]);
            fclose(input);

            if(status)
            {
                file_name = FileHandlerGetFileName(argv[i], STAGE_PRE_PROC);
                input = FileHandlerOpenFile(file_name, "r");
                
                free(file_name);
                file_name = NULL;

                if(input)
                {
                    RunAssembler(input, argv[i]);
                    fclose(input);
                }
                else 
                {
                    printf("could not open the processed file. SW is s!@#\n");
                }
            }
            else 
            {
                printf("%sPre-processor failed for some reason in file %s%s%s", CLR_RED, CLR_YEL, argv[i], CLR_RED);
                printf(" - no support%s\n", CLR_RESET);
            }
        }
    }

    return 0;
}