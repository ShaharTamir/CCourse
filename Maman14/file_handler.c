
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

SFileHandlerData *CreateFileHandlerData(int line_len, int word_len)
{
    SFileHandlerData *fh = NULL;

    fh = (SFileHandlerData *)malloc(sizeof(SFileHandlerData));

    if(fh)
    {
        fh->line = (char *)malloc(line_len);
        if(fh->line)
        {
            fh->word = (char *)malloc(word_len);
            if(fh->word)
            {
                memset(fh->line, 0, line_len);
                memset(fh->word, 0, word_len);
                fh->line_len = line_len;
                fh->bytes_read = 0;
                fh->line_count = 0;
            }
            else
            {
                free(fh->line);
                fh->line = NULL;
            }
        }
    }

    return fh;
}

void DestroyFileHandlerData(SFileHandlerData *fh)
{
    if(fh)
    {
        if(fh->line)
        {
            free(fh->line);
            fh->line = NULL;
        }
        if(fh->word)
        {
            free(fh->word);
            fh->word = NULL;
        }

        free(fh);
        fh = NULL;
    }
}