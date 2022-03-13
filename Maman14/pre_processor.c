#define _GNU_SOURCE
#include <stdio.h> /* FILE, printf, fprintf */
#include <stdlib.h> /* malloc, free */
#include <string.h> /* memset, strcpy */

#include "basic_defs.h"
#include "file_handler.h"
#include "linked_list.h"
#include "parser.h"
#include "macro.h"
#include "pre_processor.h"

typedef struct 
{
    int ignore_line;
    int status;
    FILE *out;
    SLinkedList *macro_list;
    SFileHandlerData *fh;
}SPreProcData;

static int InitPreProc(SPreProcData *data, char *file_name);
static void DestroyPreProc(SPreProcData *data);
static int FindMacroLogic(FILE *in, SPreProcData *data);
static int AddMacroToList(SPreProcData *data);
static void SpreadMacro(FILE *in, SPreProcData *data, SMacroType *macro);
static int SetMacroEnd(FILE *in, SPreProcData *data);
static int ListStringCompare(void *macro, void *str_b, void *params);

typedef int (*PreProcFunc)(FILE *in, SPreProcData *data);
static const PreProcFunc pre_proc_states_func[] = {FindMacroLogic, SetMacroEnd};

int RunPreProcessor(FILE *in, char *file_name)
{
    SPreProcData data;
    int is_macro = FALSE;
    
    if(InitPreProc(&data, file_name))
    {
        while(data.fh->bytes_read != EOF)
        {
            ++data.fh->line_count;
            data.fh->bytes_read = getline(&data.fh->line, &data.fh->line_len, in);
            data.ignore_line = is_macro || data.fh->bytes_read == EOF; /* to handle 'endm' */
            
            if(!ParserIsLineNote(data.fh->line, data.fh->bytes_read))
            {
                for(data.fh->index = 0 ; data.fh->index < data.fh->bytes_read; ++data.fh->index)
                {
                    data.fh->index = ParserNextWord(data.fh->line, data.fh->word, data.fh->index, data.fh->bytes_read);
                    is_macro = pre_proc_states_func[is_macro](in, &data);
                }

                if(!is_macro && !data.ignore_line)
                    fprintf(data.out, "%s", data.fh->line);
            }
        }
    }
    
    DestroyPreProc(&data);

    return data.status;
}

int InitPreProc(SPreProcData *data, char *file_name)
{
    char *proc_file_name = NULL;
    int ret_val = FALSE;

    data->fh = CreateFileHandlerData(MAX_LINE_LENGTH, MAX_LABEL_NAME);
    if(data->fh)
    {
        proc_file_name = GetFileName(file_name, STAGE_PRE_PROC); /* uses malloc */  
        if(proc_file_name)
        {
            data->macro_list = LinkListCreate(NULL, ListStringCompare);
            if(data->macro_list)
            {
                data->out = OpenFile(proc_file_name, "w");
                if(data->out)
                {
                    data->ignore_line = FALSE;
                    data->status = TRUE;
                    ret_val = TRUE;
                }
            }
            
            free(proc_file_name);
            proc_file_name = NULL;
        }    
    }

    return ret_val;
}

void DestroyPreProc(SPreProcData *data)
{
    if(data)
    {
        DestroyFileHandlerData(data->fh);

        if(data->out)
        {
            fclose(data->out);
        }
        if(data->macro_list)
        {
            LinkListForEach(data->macro_list, MacroDestroyWrapper, NULL); /* release all allocations for macro names */
            LinkListDestroy(data->macro_list);
            data->macro_list = NULL;
        }
    }
}

int FindMacroLogic(FILE *in, SPreProcData *data)
{
    const char *macro_str = "macro";
    int ret_val = FALSE;
    SNode *iter;

    if(!strcmp(data->fh->word, macro_str)) /* new macro definition */
    {
        data->fh->index = ParserNextWord(data->fh->line, data->fh->word, data->fh->index, data->fh->bytes_read);
        
        if(ParserValidateName(data->fh->word))
        {
            if(NULL == LinkListFind(data->macro_list, data->fh->word, NULL))
            {
                ret_val = TRUE;
                if(!AddMacroToList(data))
                    printf("DEBUG: something went wrong %d\n", __LINE__);
            }
            else
            {
                data->status = FALSE;
                printf("%serror: macro name in line: %s%d%s is already defined%s\n", \
                    CLR_RED, CLR_YEL, data->fh->line_count, CLR_RED, CLR_WHT);
            }
        }
        else
        {
            data->status = FALSE;
            printf("%serror: invalid macro name in line: %s%d%s\n", \
                CLR_RED, CLR_YEL, data->fh->line_count, CLR_WHT);
        }
    }
    else if(NULL != (iter = LinkListFind(data->macro_list, data->fh->word, NULL)))
    {
        SpreadMacro(in, data, iter->data);
        data->ignore_line = TRUE;
    }

    return ret_val;
}

int AddMacroToList(SPreProcData *data)
{
    SMacroType *new_macro = NULL;

    new_macro = MacroCreate(MAX_LABEL_NAME);

    if(!new_macro)
    {
        return FALSE;
    }

    new_macro->start_line = data->fh->line_count;
    strcpy(new_macro->name, data->fh->word);

    LinkListAppend(data->macro_list, NodeCreate(new_macro, NULL));

    return TRUE;
}

void SpreadMacro(FILE *in, SPreProcData *data, SMacroType *macro)
{
    int i = 0;

    /* move in file to the starting line of the macro */
    ParserMoveToLineNumber(in, MAX_LINE_LENGTH, macro->start_line);
    
    /* print all macro lines straight into output file */
    for(i = 0; i < macro->end_line - macro->start_line; ++i)
    {
        getline(&data->fh->line, &data->fh->line_len, in);
        fprintf(data->out, "%s", data->fh->line);
    }

    /* move back to previous point in file */
    ParserMoveToLineNumber(in, MAX_LINE_LENGTH, data->fh->line_count);
}

int SetMacroEnd(FILE *in, SPreProcData *data)
{
    const char *macro_end_str = "endm";
    SMacroType *last_macro = NULL;

    if(!strcmp(data->fh->word, macro_end_str))
    {
        last_macro = (SMacroType*)(LinkListGetTail(data->macro_list)->data);
        last_macro->end_line = data->fh->line_count - 1;

        return FALSE;
    }

    return TRUE;  
}

int ListStringCompare(void *macro, void *str_b, void *params)
{    
    (void) params;

    return strcmp((char *)(((SMacroType*)macro)->name), (char *)str_b);
}
