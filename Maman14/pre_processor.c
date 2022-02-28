#define _GNU_SOURCE
#include <stdio.h>   /* FILE type */
#include <string.h> /* strcmp,  */
#include <stdlib.h> /* malloc, free */
#include <node.h>

#include <linked_list.h>

#include "basic_defs.h"
#include "parser.h"
#include "pre_processor.h"

#define FALSE 0
#define TRUE 1

typedef enum
{
    NEW_MACRO = DEFAULT_NUM_STATES,
    MACRO_EXIST,
    END_MACRO,
    PROCESSOR_NUM_STATES
} EPreProcessStates;

typedef struct 
{
    int start_line;
    int end_line;
    char *name;
} SMacroType;

typedef struct 
{
    int in_macro;
    int curr_line_index;
    SLinkedList *macro_list;
    FILE *parsed_file;
} SProcessorData;

/********************************
 *  LinkList compare function   *
********************************/
int ListStringCompare(void *str_a, void *str_b, void *);

/********************************
 * Pre Processor State Handlers *
 *******************************/
int CheckForMacro(StateMachineData *data);
int AddMacroToList(StateMachineData *data);
int SpreadMacro(StateMachineData *data);
int SetMacroEnd(StateMachineData *data);

/********************************
 *      Service Functions       *
 *******************************/
int InitPreProcessor(FILE * in, char *parsed_file_name, SParser** parser, SProcessorData** proc_data);
void DestroyPreProcessor();

char* RunPreProcessor(FILE *in, char *file_name)
{
    char parsed_finish[] = ".am";
    SParser *parser = NULL;
    SProcessorData *proc_data = NULL;
    char *parsed_file_name = NULL;

    parsed_file_name = (char *)malloc(strlen(file_name) + sizeof(parsed_finish) + 1); /* + 1 = '\0' */

    if(parsed_file_name)
    {
        strcpy(parsed_file_name, file_name);
        strcat(parsed_file_name, parsed_finish);
        
        if(InitPreProcessor(in, parsed_file_name, &parser, &proc_data))
        {
            ParserRun(parser);
        }
        else 
        {
            printf("init pre processor fail!\n");
            exit(1);
        } 
    }

    return parsed_file_name;
}

int ListStringCompare(void *macro, void *str_b, void *params)
{    
    (void) params;

    return strcmp((char *)(((SMacroType*)macro)->name), (char *)str_b);
}

int CheckForMacro(StateMachineData* data)
{
    char macro_string[] = "macro";
    char end_macro_string[] = "endm";
    char *curr_word = NULL;
    SParserParams *p_params = NULL;
    SProcessorData *p_data = NULL;

    p_params = (SParserParams*)data->params;
    p_data = (SProcessorData*)p_params->user_data;

    while(p_params->bytes_read != p_params->line_index)
    {
        curr_word = ParserNextWord(p_params);
        
        if(curr_word)
        {
            data->val = curr_word; /* update current word */

            if(!p_data->in_macro && !strcmp(curr_word, macro_string))
                return NEW_MACRO; /* addMacroToList */

            if(p_data->in_macro && !strcmp(curr_word, end_macro_string))
                return END_MACRO; /* setMacroEnd */

            if(NULL != LinkListFind(p_data->macro_list, curr_word, NULL))
                return MACRO_EXIST; /* spreadMacro */
        }
    }

    fprintf(p_data->parsed_file, "%s", p_params->line);

    return READ_NEW_LINE;
}

int AddMacroToList(StateMachineData* data)
{
    SParserParams *p_params = NULL;
    SProcessorData *p_data = NULL;
    SMacroType *new_macro = NULL;

    new_macro = (SMacroType*)malloc(sizeof(SMacroType));

    if(new_macro)
    {        
        p_params = (SParserParams*)data->params;
        p_data = (SProcessorData*)p_params->user_data;
        new_macro->start_line = p_params->line_count;
        
        new_macro->name = ParserNextWord(p_params);
        
        if(!new_macro->name)
        {
            /* not macro */
            free(new_macro);
            fprintf(p_data->parsed_file, "%s", p_params->line);
            return READ_NEW_LINE;
        }

        data->val = new_macro->name;
        LinkListAppend(p_data->macro_list, NodeCreate(new_macro, NULL));
        p_data->in_macro = TRUE;
    }
    else
    {
        printf("cannot allocate new macro\n");
        return FINISH_READ;
    }

    return READ_NEW_LINE;
}

int SpreadMacro(StateMachineData *data)
{
    SParserParams *p_params = NULL;
    SProcessorData *p_data = NULL;
    SMacroType *macro = NULL;
    int i = 0;

    p_params = (SParserParams*)data->params;
    p_data = (SProcessorData*)p_params->user_data;
    macro = (SMacroType*)(LinkListFind(p_data->macro_list, (char *)data->val, NULL)->data); /* no need to check because found in CheckForMacro */

    /* move in file to the starting line of the macro */
    ParserMoveToLineNumber(p_params, macro->start_line);
    
    /* print all macro lines straight into output file */
    for(i = 0; i < macro->end_line - macro->start_line; ++i)
    {
        getline(&p_params->line, &p_params->line_len, p_params->input);
        fprintf(p_data->parsed_file, "%s", p_params->line);
    }

    /* move back to previous point in file */
    ParserMoveToLineNumber(p_params, p_params->line_count);

   return READ_NEW_LINE;
}

int SetMacroEnd(StateMachineData *data)
{
    SParserParams *p_params = NULL;
    SProcessorData *p_data = NULL;
    SMacroType *last_macro = NULL;

    p_params = (SParserParams*)data->params;
    p_data = (SProcessorData*)p_params->user_data;
    last_macro = (SMacroType*)(LinkListGetTail(p_data->macro_list)->data);

    last_macro->end_line = p_params->line_count - 1;
    p_data->in_macro = FALSE;


    return READ_NEW_LINE;
}

int InitPreProcessor(FILE * in, char *parsed_file_name, SParser** parser, SProcessorData** proc_data)
{
    int ok = PARSER_SUCCESS;

    *proc_data = (SProcessorData *)malloc(sizeof(SProcessorData));

    if(!*proc_data)
        return PARSER_FAIL;
    
    (*proc_data)->macro_list = LinkListCreate(NULL, ListStringCompare);

    if(!(*proc_data)->macro_list)
        return PARSER_FAIL;

    (*proc_data)->parsed_file = fopen(parsed_file_name, "w");
    
    if(!(*proc_data)->parsed_file)
        return PARSER_FAIL;

    (*proc_data)->curr_line_index = 0;
    *parser = ParserCreate(in, *proc_data, CheckForMacro, PROCESSOR_NUM_STATES, MAX_LINE_LENGTH);

    if(!*parser)
        return PARSER_FAIL;

    ok &= ParserAddState(*parser, NEW_MACRO, AddMacroToList);
    ok &= ParserAddState(*parser, MACRO_EXIST, SpreadMacro);
    ok &= ParserAddState(*parser, END_MACRO, SetMacroEnd);

    return ok;
}