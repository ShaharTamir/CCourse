#ifndef __MACRO_H__
#define __MACRO_H__

typedef struct 
{
    int start_line;
    int end_line;
    char *name;
} SMacroType;

/* allocate and initialize a new macro. if not allocated properly - return NULL 
   max_name_len is not checked! your responsibility! */
SMacroType *MacroCreate(int max_name_len);

/* properly release macro memory allocation */
void MacroDestroy(SMacroType *macro);

/* This function is an ActionFunc for LinkListForEach 
 * to properly release macro memory allocation before destroy list. */
int MacroDestroyWrapper(void *data, void *params);

/* compare function for data structures (linked_list) */
int MacroCompareName(void *macro, void *str_b, void *params);

#endif /* __MACRO_H__ */