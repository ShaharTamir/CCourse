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

/* check if function and given parameters are valid and update num of encode instructions blk  */
int FunctionValidateFunc(SFileHandlerData *fh, int *num_encode_blocks, int func);

/* returns the accessing method of word. Use this only after know valid! */
EAccessMeth FunctionGetAccessingMethod(char *word);

#endif /* __FUNCTIONS_H__ */