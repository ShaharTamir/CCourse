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

char *FileHandlerGetFileName(const char *file_name, EProgStage stage);

FILE *FileHandlerOpenFile(const char *file_name, const char *mode);

void FileHandlerRemoveAll(const char *file_name);

SFileHandlerData *FileHandlerCreate(int line_len, int word_len);

void FileHandlerDestroy(SFileHandlerData *fh);

#endif /* __FILE_HNALDER_H__ */