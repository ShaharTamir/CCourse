#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

#include "file_handler.h"

typedef enum
{
    AC_IMMEDIATE = 1, /* 0 is saved for FALSE */
    AC_DIRECT,
    AC_INDEX,
    AC_REG
}EAccessMeth;

int FunctionValidateFunc(SFileHandlerData *fh, int *num_encode_blocks, int func);

int GetAccessingMethod(char *word);

#endif /* __FUNCTIONS_H__ */