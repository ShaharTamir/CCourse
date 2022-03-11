
#include <string.h>
#include <stdlib.h>

#include <colors.h>
#include "file_handler.h"

char *GetFileName(char *file_name, EProgStage stage)
{
    static char *stages_end[] = {".as", ".am", ".obj", ".ext", ".ent"};
    char *new_file_name = NULL;

    new_file_name = (char *)malloc(strlen(file_name) + strlen(stages_end[stage]) + 1); /* + 1 = '\0' */

    if(new_file_name)
    {
        strcpy(new_file_name, file_name);
        strcat(new_file_name, stages_end[stage]);
    }

    return new_file_name;
}

FILE *OpenFile(const char *file_name, const char *mode)
{
    FILE *ret = NULL;

    ret = fopen(file_name, mode);

    if(!ret)
    {
        printf("%serror: could not open file - ", CLR_RED);
        printf("%s%s\n%s", CLR_YEL, file_name, CLR_WHT);
    }

    return ret;
}