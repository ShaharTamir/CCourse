#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>

#include <state_machine.h>

#include "basic_defs.h"
#include "pre_processor.h"

typedef struct 
{
    int line;
    char *name;
} SMacroType;

typedef enum
{
    NEW_MACRO,
    MACRO_EXIST,
    NO_MACRO
} EMacroStatus;

FILE* RunPreProcessor(FILE *in, char *file_name)
{
    char line[MAX_LINE_LENGTH] = {0};
    char macro_string[] = "macro";
    char endm_string[] = "endm";
    size_t max_read_size = MAX_LINE_LENGTH;
    int curr_line_num;
    int key = 0;
    EMacroStatus is_macro;

/*  
1. look for macro_string or macro names
2. found a new macro: save its name and line
3. find its end
*/     
    while(EOF != getline(&line, &max_read_size, in))
    {
        is_macro = IsMacro(line, &key);

        switch(is_macro)
        {
        case MACRO_EXIST:
            WriteMacroToFile(key);
            break;
        case NEW_MACRO:    
            char* macro_name = ParseMacro(in, macro_string);
            if(macro_name)
            {
                if(MacroNameValid(macro_name))
                {
                    key = hash.HashFunc(macros, macro_name);
                    if(key == -1)
                    {
                        key = HashAddItem(macro_string, macro_name);
                        WriteMacroToFile(key);
                    }
                    else
                    {
                        printf("error - macro name is already used.\n");
                    }
                }
            }    
            break;
        default:
            WriteLineToFile(line);
            break;    
        }
    }            
*/
}