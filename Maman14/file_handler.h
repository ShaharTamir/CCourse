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

/* allocate file name with the right ".end" according to program stage 
   if 'file_name' is not allocated properly - return NULL */
char *FileHandlerGetFileName(const char *file_name, EProgStage stage);

/* if file is not opened properly - return NULL */
FILE *FileHandlerOpenFile(const char *file_name, const char *mode);

/* delete all files generated at encoding stage - OBJ, EXT, ENT */
void FileHandlerRemoveAll(const char *file_name);

/* allocate a SFileHandlerData and initialize.
   if not allocated properly - return NULL */
SFileHandlerData *FileHandlerCreate(int line_len, int word_len);

/* free all data and release memory properly */
void FileHandlerDestroy(SFileHandlerData *fh);

#endif /* __FILE_HNALDER_H__ */