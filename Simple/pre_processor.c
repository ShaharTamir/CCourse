#include <stdio.h>

#include "basic_defs.h"
#include "file_handler.h"
#include "linked_list.h"
#include "parser.h"
#include "pre_processor.h"

typedef struct 
{
    int start_line;
    int end_line;
    char *name;
} SMacroType;

static int ListStringCompare(void *macro, void *str_b, void *params);

int RunPreProcessor(FILE *in, char *file_name)
{
    
    FILE *out = NULL;
    SLinkedList *macro_list = NULL;
    int bytes_read = 0, index = 0, line_count = 0;
    
    if(InitPreProc(&macro_list, &out, file_name))
    {
        while(bytes_read != EOF)
        {
            
        }
        /*  PRE_PROC_LOGIC
        
        
        
        */
    }
    
    /*DestroyPreProc();*/
}

int InitPreProc(SLinkedList **list, FILE **out, char *file_name)
{
    char *proc_file_name = NULL;
    int ret_val = FALSE;

    proc_file_name = GetFileName(file_name, STAGE_PRE_PROC);   
    if(proc_file_name)
    {
        *list = LinkListCreate(NULL, ListStringCompare);
        if(*list)
        {
            *out = OpenFile(proc_file_name, "w");
            if(*out)
                ret_val = TRUE;
        }
    }    

    return ret_val;
}

int ListStringCompare(void *macro, void *str_b, void *params)
{    
    (void) params;

    return strcmp((char *)(((SMacroType*)macro)->name), (char *)str_b);
}
