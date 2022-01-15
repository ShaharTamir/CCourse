#define _GNU_SOURCE
#include <stdio.h> /* printf */
#include <stdlib.h> /* malloc, free */

#include <state_machine.h>
#include "parser.h"

#define MAX(a, b) (a > b ? a : b)

struct _SParser
{
    StateMachineType *machine;
    StateMachineData *machine_data;
} _SParser;

/* INIT FUNCTIONS */
static int CreateAll(SParser** parser, int num_states, int max_line_len);
static int InitStatesHandlers(StateMachineType* machine);

/* BASIC STATE MACHINE HANDLERS FUNCTIONS */
static int HandleReadLines(StateMachineData* data);
static int HandleParseLines(StateMachineData* data);
static int HandleFinishRead(StateMachineData* data);


/*******************************
*         API FUNCTIONS
*******************************/
SParser* ParserCreate(FILE* input, void *data, \
    ParseFunction line_parser, int num_states, size_t max_line_len)
{
    SParser* new_parser = NULL;
    SParserParams* params = NULL;

    if(PARSER_FAIL == CreateAll(&new_parser, num_states, max_line_len))
    {
        printf("basic allocation fail. exit.\n");
        ParserDestroy(new_parser);
        return NULL;
    }

    params = new_parser->machine_data->params;
    /* init state machine data */
    params->line_len = max_line_len;
    params->bytes_read = 0;
    params->line_index = 0;
    params->user_data = data;
    params->input = input;
    params->line_parse_func = line_parser;

    if(STATE_MACHINE_FAIL == InitStatesHandlers(new_parser->machine))
    {
        printf("add basic parser states fail. exit.\n");
        ParserDestroy(new_parser);
        return NULL;
    }
    
    return new_parser;
}

void ParserDestroy(SParser* parser)
{
    SParserParams* params = NULL;
    if(parser)
    {
        if(parser->machine)
        {
            StateMachineDestroy(parser->machine);
            parser->machine = NULL;

            if(parser->machine_data)
            {
                if(parser->machine_data->params)
                {
                    params = (SParserParams*)parser->machine_data->params;
                    if(params->line)
                    {
                        free(params->line);
                        params->line = NULL;
                    }
                    params = NULL;

                    free(parser->machine_data->params);
                    parser->machine_data->params = NULL;
                    parser->machine_data->val = NULL;
                    parser->machine_data->return_val = NULL;
                }

                free(parser->machine_data);
                parser->machine_data = NULL;
            }
        }
    }
}

int ParserAddState(SParser *parser, int state_index, StateHandler handler)
{
    return StateMachineAddState(parser->machine, state_index, handler);
}

int ParserRun(SParser *parser)
{
    return StateMachineRun(parser->machine, parser->machine_data);
}

/*******************************
*       INIT FUNCTIONS
*******************************/
int CreateAll(SParser** parser, int num_states, int max_line_len)
{
    *parser = (SParser*)malloc(sizeof(SParser));

    if(*parser == NULL)
        return PARSER_FAIL;

    (*parser)->machine = StateMachineCreate(READ_NEW_LINE, MAX(DEFAULT_NUM_STATES, num_states));
    
    if((*parser)->machine == NULL)
        return PARSER_FAIL;
    
    (*parser)->machine_data = (StateMachineData*)malloc(sizeof(StateMachineData));

    if((*parser)->machine_data == NULL)
        return PARSER_FAIL;

    (*parser)->machine_data->params = (SParserParams*)malloc(sizeof(SParserParams));

    if((*parser)->machine_data->params == NULL)
        return PARSER_FAIL;

    ((SParserParams*)((*parser)->machine_data->params))->line = (char*) malloc(max_line_len);

    if(((SParserParams*)((*parser)->machine_data->params))->line == NULL)
        return PARSER_FAIL;

    return PARSER_SUCCESS;
}

int InitStatesHandlers(StateMachineType* machine)
{
    int add_status = STATE_MACHINE_OK;

    add_status &= StateMachineAddState(machine, READ_NEW_LINE, HandleReadLines);
    add_status &= StateMachineAddState(machine, PARSE_LINE, HandleParseLines);
    add_status &= StateMachineAddState(machine, FINISH_READ, HandleFinishRead);
    return add_status;
}


/*************************************
*  BASIC STATE MACHINE HANDLERS FUNCTIONS 
*************************************/
int HandleReadLines(StateMachineData* data)
{
    SParserParams *parser_data = NULL;
    
    parser_data = (SParserParams*)data->params; /* assign ptr to local var to ease access to parser data */

    parser_data->line_index = 0;
    parser_data->bytes_read = getline(&parser_data->line, &parser_data->line_len, parser_data->input);

    if(EOF == parser_data->bytes_read)
    {
        return FINISH_READ;
    }

    return PARSE_LINE;
}

int HandleParseLines(StateMachineData* data)
{
    SParserParams *parser_data = NULL;
    int next_state = PARSE_LINE;
    parser_data = (SParserParams*)data->params;

    while(parser_data->line_index < parser_data->bytes_read) /* read until line is finished */ 
    {
        next_state = parser_data->line_parse_func(data);
        if(next_state != PARSE_LINE)
        {
            return next_state;
        }

        ++parser_data->line_index;
    }

    return READ_NEW_LINE;
}  

int HandleFinishRead(StateMachineData* data)
{   
    /*SParserParams *parser_data;
    parser_data = (SParserParams *)data->params;

    
    if(parser_data->print_function)
        parser_data->print_function(data);
    */
   (void) data;

    return STATE_MACHINE_END_STATE;
}

