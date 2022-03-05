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

char *GetFileName(char *file_name, EProgStage stage);

FILE *OpenFile(const char *file_name, const char *mode);

#endif /* __FILE_HNALDER_H__ */