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

/*
struct SPreProcData
{
    FILE *out;
    SLinkedList *macro_list;
    char line[MAX_LINE_LENGTH];
    char word[MAX_LABEL_NAME];
    size_t line_len;
    int bytes_read;
    int index;
    int line_count;
};
*/

static int ListStringCompare(void *macro, void *str_b, void *params);

int RunPreProcessor(FILE *in, char *file_name)
{
    FILE *out = NULL; SLinkedList *macro_list = NULL; SNode *iter = NULL;
    const char *macro_str = "macro";
    const char *macro_end_str = "endm";
    char line[MAX_LINE_LENGTH] = {0};
    char word[MAX_LABEL_NAME] = {0};
    size_t line_len = MAX_LINE_LENGTH;
    int bytes_read = 0, index = 0, line_count = 0; 
    int is_macro = FALSE, ret_val = TRUE;
    
    if(InitPreProc(&macro_list, &out, file_name))
    {
        while(bytes_read != EOF && ret_val == TRUE)
        {
            ++line_count;
            bytes_read = getline(&line, &line_len, in);
            
            if(!ParserIsLineNote(line, bytes_read))
            {
                for(index = 0 ; index < bytes_read; ++index)
                {
                    ParserNextWord(line, word, &index, bytes_read);
                    
                    if(!is_macro)
                    {   
                        if(!strcmp(word, macro_str)) /* new macro definition */
                        {
                            ParserNextWord(line, word, &index, bytes_read);
                            
                            if(ParserValidateName(word))
                            {
                                is_macro = TRUE;
                                if(!AddMacroToList(macro_list, word, line_count))
                                    printf("something went wrong %d\n", __LINE__);
                            }
                            else
                            {
                                printf("%sinvalid macro name in line: %s%d%s\n", CLR_RED, CLR_YEL, line_count, CLR_WHT);
                                ret_val = FALSE;
                            }
                        }
                        else if(NULL != (iter = LinkListFind(macro_list, word, NULL)))
                        {
                            SpreadMacro(in, out, iter->data, line_count);
                        }
                        else
                        {
                            fprintf(out, "%s", line);
                        }
                    } 
                    else if(!strcmp(word, macro_end_str))
                    {
                        SetMacroEnd(macro_list, line_count);
                    }
                }
            }
        }
    }
    
    DestroyPreProc(&macro_list, &out);
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
