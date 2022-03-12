#ifndef __FILE_HNALDER_H__
#define __FILE_HNALDER_H__

#include <stdio.h>

typedef enum
{
    STAGE_FIRST,
    STAGE_PRE_PROC,
    STAGE_OBJ,
    STAGE_EXT,
    STAGE_ENT,
    NUM_PROG_STAGES
} EProgStage;

typedef struct 
{
    int bytes_read;
    int index;
    int line_count;
    size_t line_len;
    char *line;
    char *word;
} SFileHandlerData;

char *GetFileName(char *file_name, EProgStage stage);

FILE *OpenFile(const char *file_name, const char *mode);

SFileHandlerData *CreateFileHandlerData(int line_len, int word_len);

void DestroyFileHandlerData(SFileHandlerData *fh);

#endif /* __FILE_HNALDER_H__ */