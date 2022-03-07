#include <stdio.h> /* FILE, printf, fprintf */
#include <string.h> /* memset */

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

typedef struct 
{
    FILE *out;
    SLinkedList *macro_list;
    char line[MAX_LINE_LENGTH];
    char word[MAX_LABEL_NAME];
    size_t line_len;
    int bytes_read;
    int index;
    int line_count;
    int status;
}SPreProcData;

static int ListStringCompare(void *macro, void *str_b, void *params);
static int InitPreProc(SPreProcData *data, char *file_name);
static int FindMacroLogic(FILE *in, SPreProcData *data);
static int SetMacroEnd(SPreProcData *data);

typedef int (*PreProcFunc)(FILE *in, SPreProcData *data);
static const PreProcFunc pre_proc_states_func[] = {FindMacroLogic, SetMacroEnd};

int RunPreProcessor(FILE *in, char *file_name)
{
    SPreProcData data;
    size_t line_len = MAX_LINE_LENGTH;
    int is_macro = FALSE;
    
    if(InitPreProc(&data, file_name))
    {
        while(data.bytes_read != EOF && data.status == TRUE)
        {
            ++data.line_count;
            data.bytes_read = getline(&data.line, &line_len, in);
            
            if(!ParserIsLineNote(data.line, data.bytes_read))
            {
                for(data.index = 0 ; data.index < data.bytes_read; ++data.index)
                {
                    data.index = ParserNextWord(data.line, data.word, &data.index, data.bytes_read);
                    is_macro = pre_proc_states_func[is_macro](in, &data);
                }
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

    proc_file_name = GetFileName(file_name, STAGE_PRE_PROC);   
    if(proc_file_name)
    {
        data->macro_list = LinkListCreate(NULL, ListStringCompare);
        if(data->macro_list)
        {
            data->out = OpenFile(proc_file_name, "w");
            if(data->out)
            {
                data->status = TRUE;
                memset(data->line, 0, MAX_LINE_LENGTH);
                memset(data->word, 0, MAX_LABEL_NAME);
                ret_val = TRUE;
            }
        }
    }    

    return ret_val;
}

int FindMacroLogic(FILE *in, SPreProcData *data)
{
    char *macro_str = "macro";
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
    }
    else
    {
        fprintf(data->out, "%s", data->line);
    }

    return data->status;
}

int SetMacroEnd(SPreProcData *data)
{
    const char *macro_end_str = "endm";

    if(!strcmp(data->word, macro_end_str))
    {


        return FALSE;
    }

    return TRUE;  
}

int ListStringCompare(void *macro, void *str_b, void *params)
{    
    (void) params;

    return strcmp((char *)(((SMacroType*)macro)->name), (char *)str_b);
}
