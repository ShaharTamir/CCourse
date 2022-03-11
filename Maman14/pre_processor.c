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
    FILE *out;
    SLinkedList *macro_list;
    char *line;
    char *word;
    size_t line_len;
    int bytes_read;
    int index;
    int line_count;
    int ignore_line;
    int status;
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
        while(data.bytes_read != EOF && data.status == TRUE)
        {
            ++data.line_count;
            data.bytes_read = getline(&data.line, &data.line_len, in);
            data.ignore_line = is_macro; /* to handle 'endm' */
            
            if(!ParserIsLineNote(data.line, data.bytes_read))
            {
                for(data.index = 0 ; data.index < data.bytes_read; ++data.index)
                {
                    data.index = ParserNextWord(data.line, data.word, data.index, data.bytes_read);
                    is_macro = pre_proc_states_func[is_macro](in, &data);
                }

                if(!is_macro && !data.ignore_line)
                    fprintf(data.out, "%s", data.line);
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

    data->line = (char *)malloc(MAX_LINE_LENGTH);
    if(data->line)
    {
        data->word = (char *)malloc(MAX_LABEL_NAME);
        if(data->word)
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
                        memset(data->line, 0, MAX_LINE_LENGTH);
                        memset(data->word, 0, MAX_LABEL_NAME);
                        data->line_len = MAX_LINE_LENGTH;
                        data->bytes_read = data->line_count = 0;
                        data->ignore_line = FALSE;
                        data->status = TRUE;
                        ret_val = TRUE;
                    }
                }
                
                free(proc_file_name);
                proc_file_name = NULL;
            }    
        }
    }

    return ret_val;
}

void DestroyPreProc(SPreProcData *data)
{
    if(data)
    {
        if(data->line)
        {
            free(data->line);
            data->line = NULL;
        }
        if(data->word)
        {
            free(data->word);
            data->word = NULL;
        }
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

    if(!strcmp(data->word, macro_str)) /* new macro definition */
    {
        data->index = ParserNextWord(data->line, data->word, data->index, data->bytes_read);
        
        if(ParserValidateName(data->word))
        {
            if(NULL == LinkListFind(data->macro_list, data->word, NULL))
            {
                ret_val = TRUE;
                if(!AddMacroToList(data))
                    printf("DEBUG: something went wrong %d\n", __LINE__);
            }
            else
            {
                data->status = FALSE;
                printf("%serror: macro name in line: %s%d%s is already defined%s\n", \
                    CLR_RED, CLR_YEL, data->line_count, CLR_RED, CLR_WHT);
            }
        }
        else
        {
            data->status = FALSE;
            printf("%serror: invalid macro name in line: %s%d%s\n", \
                CLR_RED, CLR_YEL, data->line_count, CLR_WHT);
        }
    }
    else if(NULL != (iter = LinkListFind(data->macro_list, data->word, NULL)))
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

    new_macro->start_line = data->line_count;
    strcpy(new_macro->name, data->word);

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
        getline(&data->line, &data->line_len, in);
        fprintf(data->out, "%s", data->line);
    }

    /* move back to previous point in file */
    ParserMoveToLineNumber(in, MAX_LINE_LENGTH, data->line_count);
}

int SetMacroEnd(FILE *in, SPreProcData *data)
{
    const char *macro_end_str = "endm";
    SMacroType *last_macro = NULL;

    if(!strcmp(data->word, macro_end_str))
    {
        last_macro = (SMacroType*)(LinkListGetTail(data->macro_list)->data);
        last_macro->end_line = data->line_count - 1;

        return FALSE;
    }

    return TRUE;  
}

int ListStringCompare(void *macro, void *str_b, void *params)
{    
    (void) params;

    return strcmp((char *)(((SMacroType*)macro)->name), (char *)str_b);
}
