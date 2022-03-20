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

/* search for new 'macro' start definitions in line. 
   if found return TRUE and set the state of pre-proc into "in macro" */
static int FindMacroLogic(FILE *in, SPreProcData *data);

/* add new macro to list of macros. if macro is not added return FALSE */
static int AddMacroToList(SPreProcData *data);

/* switch macro name into the macro code definition */
static void SpreadMacro(FILE *in, SPreProcData *data, SMacroType *macro);

/* search if reached end of macro definition - set relevant data to macro defintion 
   if found return FALSE to set the state of the pre-proc into "not in macro",
   else return TRUE - means still in macro. */
static int SetMacroEnd(FILE *in, SPreProcData *data);

/* pre_proc_states are FALSE==not_in_macro or TRUE==in_macro function handlers are set in order */
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
            
            if(!ParserIsLineNote(data.fh->line, data.fh->bytes_read)) /* ignore note or empty lines */
            {
                for(data.fh->index = 0 ; data.fh->index < data.fh->bytes_read; ++data.fh->index) /* parse line untill finish */
                {
                    data.fh->index = ParserNextWord(data.fh->line, data.fh->word, data.fh->index, data.fh->bytes_read);
                    is_macro = pre_proc_states_func[is_macro](in, &data); /* call logic function according to is_macro status */
                }

                if(!is_macro && !data.ignore_line)
                    fprintf(data.out, "%s", data.fh->line);
            }
        }
    }
    
    DestroyPreProc(&data);

    if(is_macro)
        data.status = FALSE;

    return data.status;
}

int InitPreProc(SPreProcData *data, char *file_name)
{
    char *proc_file_name = NULL;
    int ret_val = FALSE;

    data->fh = FileHandlerCreate(MAX_LINE_LENGTH, MAX_LINE_LENGTH);
    if(data->fh)
    {
        proc_file_name = FileHandlerGetFileName(file_name, STAGE_PRE_PROC); /* uses malloc */  
        if(proc_file_name)
        {
            data->macro_list = LinkListCreate(NULL, MacroCompareName);
            if(data->macro_list)
            {
                data->out = FileHandlerOpenFile(proc_file_name, "w");
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
        FileHandlerDestroy(data->fh);

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
            if(NULL == LinkListFind(data->macro_list, data->fh->word, NULL)) /* found new valid macro definition */
            {
                ret_val = TRUE;
                if(!AddMacroToList(data))
                    printf("DEBUG: something went wrong %d\n", __LINE__);
            }
            else
            {
                ERR_AT("macro name is already defined", data->fh->line_count);
                data->status = FALSE;
            }
        }
        else
        {
            ERR_AT("invalid macro name", data->fh->line_count);
            data->status = FALSE;
        }
    }
    else if(NULL != (iter = LinkListFind(data->macro_list, data->fh->word, NULL))) /* found macro we already know */
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
