#include <stdlib.h> /* malloc, free */
#include <string.h> /* memset */
#include "basic_defs.h" /* TRUE */
#include "macro.h"

SMacroType *MacroCreate(int max_name_len)
{
    SMacroType *new_macro = NULL;

    new_macro = (SMacroType *)malloc(sizeof(SMacroType));
    
    if(!new_macro)
        return NULL;

    new_macro->name = (char *)malloc(max_name_len);

    if(!new_macro->name)
    {
        free(new_macro);
        new_macro = NULL;
        return NULL;
    }

     /* init macro data */
    new_macro->start_line = 0;
    new_macro->end_line = 0;
    memset(new_macro->name, 0, max_name_len);

    return new_macro;
}

void MacroDestroy(SMacroType *macro)
{
    if(macro)
    {
        if(macro->name)
        {
            free(macro->name);
            macro->name = NULL;
        }

        free(macro);
        macro = NULL;
    }
}

int MacroDestroyWrapper(void *data, void *params)
{
    SMacroType *macro = NULL;
    macro = (SMacroType *)data;

    MacroDestroy(macro);

    return TRUE;
}