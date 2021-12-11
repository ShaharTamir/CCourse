#define _GNU_SOURCE
#include <stdio.h> /* printf */
#include <stdlib.h> /* malloc, free */

#include <test_input.h>
#include <stack.h>
#include <state_machine.h>

#define MAX_LINE_INPUT 100
#define STACK_STARTING_SIZE 20
#define PARSER_FAIL -1
#define PARSER_SUCCESS 0
#define TRUE 1
#define FALSE 0

typedef enum 
{
    READ_NEW_LINE,
    PARSE_LINE,
    SPECIAL_CHARACTER,
    OPEN_PARENTHES,
    CLOSE_PARENTHES,
    CLOSE_SPECIAL_PARENTHES,
    TXT,
    OPEN_NOTE,
    CLOSE_NOTE,
    VERIFY_LINE_DOC_BALANCE,
    FINISH_READ,
    NUM_STATES
} EParserState;

typedef struct
{
    int is_string;
    int is_note;
    int bytes_read;
    int line_index;
    Stack* doc_stack;
    Stack* line_stack;
    FILE* input;
    char *line;
} SParserParams;

/* MAIN FUNCTION */
static void RunSingleParTest(FILE* input);

/* INIT FUNCTIONS */
static int CreateAll(StateMachineType** machine, SParserParams* params);
static void DestroyAll(StateMachineType** machine, SParserParams* parser_data);
static int InitStatesHandlers(StateMachineType* machine);

/* STATE MACHINE HANDLERS FUNCTIONS */
static int HandleReadLines(StateMachineData* data);
static int HandleParseLines(StateMachineData* data);
static int HandleSpecialCharacter(StateMachineData* data);
static int HandleOpenParenthes(StateMachineData* data);
static int HandleCloseParenthes(StateMachineData* data);
static int HandleCloseSpecialParenthes(StateMachineData* data);
static int HandleTxtChar(StateMachineData* data);
static int HandleOpenNote(StateMachineData* data);
static int HandleCloseNote(StateMachineData* data);
static int HandleLineDocBalance(StateMachineData* data);
static int HandlePrintSummary(StateMachineData* data);

/* SERVICE FUNCTIONS */
static int IsParenthesMatch(char stack_top, char new_input);
static void PrintLineReport(char *line, int is_balanced);

int main(int argc, char *argv[])
{
    PrintInputInstructions(MAX_LINE_INPUT);
    HandleInput(argc, argv, &RunSingleParTest);

    return 0;
}

void RunSingleParTest(FILE* input)
{
    StateMachineType *machine = NULL;
    StateMachineData machine_data = {NULL, NULL, NULL};
    SParserParams parser_data;
    char val = 0;
    int ret_val = PARSER_SUCCESS;

    if(PARSER_FAIL == CreateAll(&machine, &parser_data))
    {
        printf("basic allocation fail. exit.\n");
        DestroyAll(&machine, &parser_data);
        return;
    }

    /* init state machine data */
    parser_data.is_note = FALSE;
    parser_data.is_string = FALSE;
    parser_data.bytes_read = 0;
    parser_data.line_index = 0;
    parser_data.input = input;
    machine_data.val = &val;
    machine_data.params = &parser_data;
    machine_data.return_val = &ret_val;

    if(STATE_MACHINE_FAIL == InitStatesHandlers(machine))
    {
        printf("add states fail. exit.\n");
        DestroyAll(&machine, &parser_data);
        return;
    }

    /* Start running the parser after initiating everything */
    StateMachineRun(machine, &machine_data); 
    
    DestroyAll(&machine, &parser_data);
}

/*******************************
*       INIT FUNCTIONS
*******************************/
int CreateAll(StateMachineType** machine, SParserParams* parser_data)
{
    *machine = StateMachineCreate(READ_NEW_LINE, NUM_STATES);
    
    if(*machine == NULL)
        return PARSER_FAIL;
    
    parser_data->doc_stack = StackCreate(STACK_STARTING_SIZE, sizeof(char));

    if(parser_data->doc_stack == NULL)
        return PARSER_FAIL;

    parser_data->line_stack = StackCreate(STACK_STARTING_SIZE, sizeof(char));

    if(parser_data->line_stack == NULL)
        return PARSER_FAIL;

    parser_data->line = (char *) malloc(MAX_LINE_INPUT);
    if(parser_data->line == NULL)
        return PARSER_FAIL;

    return PARSER_SUCCESS;
}

int InitStatesHandlers(StateMachineType* machine)
{
    int add_status = STATE_MACHINE_OK;

    add_status &= StateMachineAddState(machine, READ_NEW_LINE, HandleReadLines);
    add_status &= StateMachineAddState(machine, PARSE_LINE, HandleParseLines);
    add_status &= StateMachineAddState(machine, SPECIAL_CHARACTER, HandleSpecialCharacter);
    add_status &= StateMachineAddState(machine, OPEN_PARENTHES, HandleOpenParenthes);
    add_status &= StateMachineAddState(machine, CLOSE_PARENTHES, HandleCloseParenthes);
    add_status &= StateMachineAddState(machine, CLOSE_SPECIAL_PARENTHES, HandleCloseSpecialParenthes);
    add_status &= StateMachineAddState(machine, TXT, HandleTxtChar);
    add_status &= StateMachineAddState(machine, OPEN_NOTE, HandleOpenNote);
    add_status &= StateMachineAddState(machine, CLOSE_NOTE, HandleCloseNote);
    add_status &= StateMachineAddState(machine, VERIFY_LINE_DOC_BALANCE, HandleLineDocBalance);
    add_status &= StateMachineAddState(machine, FINISH_READ, HandlePrintSummary);
    return add_status;
}

void DestroyAll(StateMachineType** machine, SParserParams* parser_data)
{
    if(*machine)
        StateMachineDestroy(*machine);
    if(parser_data->doc_stack)
        StackDestroy(parser_data->doc_stack);
    if(parser_data->line_stack)
        StackDestroy(parser_data->line_stack);
    if(parser_data->line)
    {
        free(parser_data->line);
        parser_data->line = NULL;
    }
}

/*************************************
*  STATE MACHINE HANDLERS FUNCTIONS 
*************************************/
int HandleReadLines(StateMachineData* data)
{
    SParserParams *parser_data = NULL;
    size_t max_read_size = MAX_LINE_INPUT;
    
    parser_data = (SParserParams*)data->params; /* assign ptr to local var to ease access to parser data */

    StackEmptyStack(parser_data->line_stack); /* empty the line stack before each read */
    parser_data->is_string = FALSE;
    parser_data->line_index = 0;
    parser_data->bytes_read = getline(&parser_data->line, &max_read_size, parser_data->input);

    if(EOF == parser_data->bytes_read)
    {
        return FINISH_READ;
    }

    return PARSE_LINE;
}

int HandleParseLines(StateMachineData* data)
{
    static char special_characters[] = {'{', '(', '[', ']', ')', '}', '"', '/', '*', EOF};
    SParserParams *parser_data = NULL;
    int spec_char_index = 0;

    parser_data = (SParserParams*)data->params;

    while(parser_data->line_index < parser_data->bytes_read) /* read until line is finished */ 
    {
        for(spec_char_index = 0; spec_char_index < sizeof(special_characters); ++spec_char_index)
        {
            if(parser_data->line[parser_data->line_index] == special_characters[spec_char_index])
            {
                *(char *)data->val = parser_data->line[parser_data->line_index];
                ++parser_data->line_index;
                return SPECIAL_CHARACTER;
            }
        }

        ++parser_data->line_index;
    }

    PrintLineReport(parser_data->line, StackIsEmpty(parser_data->line_stack));

    if(!StackIsEmpty(parser_data->line_stack))
        return VERIFY_LINE_DOC_BALANCE;

    return READ_NEW_LINE;
}

int HandleSpecialCharacter(StateMachineData* data)
{
    switch(*(char *)data->val)
    {
        case '{':
        case '(':
        case '[':
            return OPEN_PARENTHES;
        case '}':
            return CLOSE_SPECIAL_PARENTHES;
        case ')':
        case ']':
            return CLOSE_PARENTHES;
        case '"':
            return TXT;
        case '/':
            return OPEN_NOTE;
        case '*':
            return CLOSE_NOTE;
        case EOF:
            return FINISH_READ;
        default:
            printf("reached invalid option at %d with %c. quit machine.\n", __LINE__, *(char *)data->val);
            return STATE_MACHINE_END_STATE; 
    }
}

int HandleOpenParenthes(StateMachineData* data)
{
    SParserParams *parser_data;
    parser_data = (SParserParams*)data->params;

    if(!parser_data->is_string && !parser_data->is_note)
    {
        if('{' == *(char*)data->val)
            StackPush(parser_data->doc_stack, data->val); /* doc stack follows only the {} parenthes */
            
        StackPush(parser_data->line_stack, data->val);
    }

    return PARSE_LINE;
}

int HandleCloseParenthes(StateMachineData* data)
{
    SParserParams *parser_data;
    char stack_top = 0;
    char pop_output = 0;

    parser_data = (SParserParams*)data->params;
    
    if(parser_data->is_string  || parser_data->is_note) /* string or note ignore parenthes */
    {
        return PARSE_LINE;
    }

    if(StackIsEmpty(parser_data->line_stack)) 
    {
        /* found ']' or ')' without open - document is unbalanced */
        *(int*)data->return_val = PARSER_FAIL;
        StackPush(parser_data->line_stack, data->val);
    }
    else
    {
        StackPeek(parser_data->line_stack, &stack_top);
        if(IsParenthesMatch(stack_top, *(char*)data->val))
            StackPop(parser_data->line_stack, &pop_output);
        else /* parenthes do not match */
        {
            /* found ']' or ')' without open match - document is unbalanced */
            *(int*)data->return_val = PARSER_FAIL;
            StackPush(parser_data->line_stack, data->val);
        }
    }

    return PARSE_LINE;
}

int HandleCloseSpecialParenthes(StateMachineData* data)
{
    SParserParams *parser_data;
    char stack_top = 0;
    char pop_output = 0;

    parser_data = (SParserParams*)data->params;
    
    if(parser_data->is_string  || parser_data->is_note)
    {
        return PARSE_LINE;
    }

    StackPeek(parser_data->doc_stack, &stack_top);
    if(IsParenthesMatch(stack_top, *(char*)data->val))
    {
        StackPop(parser_data->doc_stack, &pop_output);
        
        if(StackIsEmpty(parser_data->line_stack)) 
        {
            StackPush(parser_data->line_stack, data->val); /* means spec line is not balanced but doc might be */
        }
        else
        {
            StackPeek(parser_data->line_stack, &stack_top);
            if(IsParenthesMatch(stack_top, *(char*)data->val))
                StackPop(parser_data->line_stack, &pop_output);
            else
                StackPush(parser_data->line_stack, data->val); /* means spec line is not balanced but doc might be */
        }
    }
    else
    {
        StackPush(parser_data->doc_stack, data->val); /* found a '}' without '{' before him. doc will be unbalanced. */
        StackPush(parser_data->line_stack, data->val); /* if '}' didn't have '{' before, then surely line is unbalanced */
    }    

    return PARSE_LINE;
}

int HandleTxtChar(StateMachineData* data)
{
    SParserParams *parser_data;
    parser_data = (SParserParams*)data->params;

    if(parser_data->is_string) /* reached end of "string" */
    {
        parser_data->is_string = FALSE;
    }
    else if(!parser_data->is_note)
    {
        parser_data->is_string = TRUE;
    }

    return PARSE_LINE;
}

int HandleOpenNote(StateMachineData* data)
{
    SParserParams *parser_data;
    parser_data = data->params;

    /* if not already in string or note, and next char in line completes note sign */
    if(!parser_data->is_string && !parser_data->is_note &&
        '*' == parser_data->line[parser_data->line_index])
    {
      parser_data->is_note = TRUE;
      ++parser_data->line_index; /* handled this line index. move to next. */
    }

    return PARSE_LINE;
}   

int HandleCloseNote(StateMachineData* data)
{
    SParserParams *parser_data;
    parser_data = (SParserParams*)data->params;

    /* not already in string and possible reach to end on note */
    if(!parser_data->is_string && '/' == parser_data->line[parser_data->line_index]) 
    {
        parser_data->is_note = FALSE; /* close note if was opened */
        ++parser_data->line_index;
    }

    return PARSE_LINE;
}

int HandleLineDocBalance(StateMachineData* data)
{
    SParserParams *parser_data;
    char pop_output = 0;

    parser_data = data->params;

    /* if doc is still valid - verify that there are no '(' or '[' that disqualify the doc 
        and are hidden in the line stack  */
    while(*(int*)data->return_val != PARSER_FAIL &&
        !StackIsEmpty(parser_data->line_stack))
    {
        StackPop(parser_data->line_stack, &pop_output);
        if(pop_output != '{' && pop_output != '}')
        {
            *(int*)data->return_val = PARSER_FAIL;
            break;
        }
    }

    return READ_NEW_LINE;
}

int HandlePrintSummary(StateMachineData* data)
{   
    SParserParams *parser_data;
    parser_data = (SParserParams *)data->params;
    
    printf("\n****** SUMMARY ******\n\n");
    printf("The input is ");

    if(!StackIsEmpty(parser_data->doc_stack) || PARSER_FAIL == *(int*)data->return_val)
    {
        printf("not ");
    }

    printf("balanced\n\n");

    return STATE_MACHINE_END_STATE;
}

/*************************************
*          SERVICE FUNCTIONS 
*************************************/
int IsParenthesMatch(char stack_top, char new_input)
{
    switch(stack_top)
    {
        case '(':
            return ')' == new_input;
        case '[':
            return ']' == new_input;
        case '{':
            return '}' == new_input;
        default:
            return 0;
    }
}

void PrintLineReport(char *line, int is_balanced)
{
    printf("This line is ");
    
    if(!is_balanced)
        printf("not ");    
    
    printf("balanced : %s\n", line);
}

