#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

#include "..\utills\test_input.h"
#include "..\utills\stack.h"
#include "..\utills\state_machine.h"

#define MAX_LINE_INPUT 100
#define STACK_LIMIT 1000
#define PARSER_FAIL -1
#define PARSER_SUCCESS 0

typedef enum 
{
    READ_NEW_LINE,
    PARSE_LINE,
    SPECIAL_CHARACTER,
    OPEN_PARENTHES,
    CLOSE_PARENTHES,
    TXT,
    OPEN_NOTE,
    CLOSE_NOTE,
    FINISH_READ,
    NUM_STATES
} EParserState;

typedef struct
{
    int bytes_read;
    int line_index;
    Stack* doc_stack;
    Stack* line_stack;
    FILE* input;
    char *line;
} SParserParams;

int isClosingParenthes(char parentehs);
int isParenthesMatch(char stackTop, char newInput);
void PrintLineReport(char *line, int isBalanced);

int ReadLines(StateMachineData* data);
int ParseLines(StateMachineData* data);
int HandleSpecialCharacter(StateMachineData* data);
int HandleOpenParenthes(StateMachineData* data);
int HandleCloseParenthes(StateMachineData* data);
int HandleTxtChar(StateMachineData* data);
int HandleOpenNote(StateMachineData* data);
int HandleCloseNote(StateMachineData* data);
int PrintIsTextBalanced(StateMachineData* data);

void RunSingleParTest(FILE* input);
static int CreateAll(StateMachineType** machine, SParserParams* params);

static char g_special_characters[] = {'{', '(', '[', ']', ')', '}', '"', '/', '*', EOF};


int main(int argc, char *argv[])
{
    printInputInstructions(MAX_LINE_INPUT);
    handleInput(argc, argv, &RunSingleParTest);

    return 0;
}

void RunSingleParTest(FILE* input)
{
    StateMachineType *machine = NULL;
    StateMachineData machine_data = {NULL, NULL, NULL};
    SParserParams parser_data;
    char val = 0;
    int add_status = STATE_MACHINE_OK;

    if(PARSER_FAIL == CreateAll(&machine, &parser_data))
    {
        return;
    }

    parser_data.bytes_read = 0;
    parser_data.line_index = 0;
    parser_data.input = input;
    machine_data.val = &val;
    machine_data.params = &parser_data;

    add_status &= AddStateMachine(machine, READ_NEW_LINE, ReadLines);
    add_status &= AddStateMachine(machine, PARSE_LINE, ParseLines);
    add_status &= AddStateMachine(machine, SPECIAL_CHARACTER, HandleSpecialCharacter);
    add_status &= AddStateMachine(machine, OPEN_PARENTHES, HandleOpenParenthes);
    add_status &= AddStateMachine(machine, CLOSE_PARENTHES, HandleCloseParenthes);
    add_status &= AddStateMachine(machine, TXT, HandleTxtChar);
    add_status &= AddStateMachine(machine, OPEN_NOTE, HandleOpenNote);
    add_status &= AddStateMachine(machine, CLOSE_NOTE, HandleCloseNote);
    add_status &= AddStateMachine(machine, FINISH_READ, PrintIsTextBalanced);

    if(add_status == STATE_MACHINE_FAIL)
    {
        DestroyStateMachine(machine);
        StackDestroy(parser_data.doc_stack);
        StackDestroy(parser_data.line_stack);
        printf("add states fail. exit.\n");
        return;
    }

    RunStateMachine(machine, &machine_data);

    DestroyStateMachine(machine);
    StackDestroy(parser_data.doc_stack);
    StackDestroy(parser_data.line_stack);
    free(parser_data.line);
    parser_data.line = NULL;
}

int CreateAll(StateMachineType** machine, SParserParams* parser_data)
{
    *machine = CreateStateMachine(READ_NEW_LINE, NUM_STATES);
    
    if(*machine == NULL)
    {
        printf("malloc fail. exit.\n");
        return PARSER_FAIL;
    }
    
    parser_data->doc_stack = StackCreate(STACK_LIMIT, sizeof(char));

    if(parser_data->doc_stack == NULL)
    {
        DestroyStateMachine(*machine);
        printf("malloc fail. exit.\n");
        return PARSER_FAIL;
    }

    parser_data->line_stack = StackCreate(MAX_LINE_INPUT, sizeof(char));

    if(parser_data->line_stack == NULL)
    {
        DestroyStateMachine(*machine);
        StackDestroy(parser_data->doc_stack);
        printf("malloc fail. exit. \n");
        return PARSER_FAIL;
    }

    parser_data->line = (char *) malloc(MAX_LINE_INPUT);
    if(parser_data->line == NULL)
    {
        DestroyStateMachine(*machine);
        StackDestroy(parser_data->doc_stack);
        StackDestroy(parser_data->line_stack);
        printf("malloc fail. exit. \n");
        return PARSER_FAIL;
    }

    return PARSER_SUCCESS;
}

int ReadLines(StateMachineData* data)
{
    SParserParams *parser_data = NULL;
    size_t max_read_size = MAX_LINE_INPUT;
    
    parser_data = (SParserParams*)data->params;

    StackEmptyStack(parser_data->line_stack);
    parser_data->bytes_read = getline(&parser_data->line, &max_read_size, parser_data->input);
    parser_data->line_index = 0;

    if(EOF == parser_data->bytes_read)
    {
        return FINISH_READ;
    }

    return PARSE_LINE;
}

int ParseLines(StateMachineData* data)
{
    SParserParams *parser_data = NULL;
    int spec_char_index = 0;

    parser_data = (SParserParams*)data->params;

    while(parser_data->line_index < parser_data->bytes_read)
    {
        for(spec_char_index = 0; spec_char_index < sizeof(g_special_characters); ++spec_char_index)
        {
            if(parser_data->line[parser_data->line_index] == g_special_characters[spec_char_index])
            {
                *(char *)data->val = parser_data->line[parser_data->line_index];
                ++parser_data->line_index;
                return SPECIAL_CHARACTER;
            }
        }

        ++parser_data->line_index;
    }

    PrintLineReport(parser_data->line, StackIsEmpty(parser_data->line_stack));

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
    SParserParams *params;
    char stack_top = 0;

    params = (SParserParams*)data->params;

    if(!StackIsEmpty(params->doc_stack))
    {
        StackPeek(params->doc_stack, &stack_top);
        if(stack_top != '"' && stack_top != '*') /* not in note and not in string */
        {
            StackPush(params->doc_stack, data->val);
            StackPush(params->line_stack, data->val);
        }
    }
    else
    {
        StackPush(params->doc_stack, data->val);
        StackPush(params->line_stack, data->val);
    }

    return PARSE_LINE;
}

int HandleCloseParenthes(StateMachineData* data)
{
    SParserParams *params;
    char stack_top = 0;
    char output = 0;

    params = (SParserParams*)data->params;
    
    if(!StackIsEmpty(params->doc_stack))
    {
        StackPeek(params->doc_stack, &stack_top);
        if(stack_top != '"' && stack_top != '*') /* means not in note and not in string */
        {
            if(isParenthesMatch(stack_top, *(char*)data->val))
            {
                StackPop(params->doc_stack, &output);
                
                if(!StackIsEmpty(params->line_stack))
                    StackPop(params->line_stack, &output);
                else
                    StackPush(params->line_stack, data->val); /* make this line report as uneven */
            }
            else /* parenthes do not match */
            {
                /* NOTE: here we can actually quit parsing because we know the result. */
                StackPush(params->doc_stack, data->val); /* reach here once makes the whole doc uneven */
                StackPush(params->line_stack, data->val); /* if doc is not even, this line is surely uneven */
            }
        } /* else do nothing */
    }
    else /* means both are surely empty make doc + line uneven */
    {
        /* NOTE: here we can actually quit parsing because we know the result. */
        StackPush(params->doc_stack, data->val);
        StackPush(params->line_stack, data->val);
    }

    return PARSE_LINE;
}

int HandleTxtChar(StateMachineData* data)
{
    SParserParams *params;
    char stack_top = 0;
    char output = 0;

    params = data->params;

    if(!StackIsEmpty(params->doc_stack))
    {
        StackPeek(params->doc_stack, &stack_top);
        if('"' == stack_top) /* reached end of "string". clean from stack */
        {
            StackPop(params->doc_stack, &output);
        }
        else if('*' != stack_top) /* if not in a note */
        {
            StackPush(params->doc_stack, data->val);
        }
    }
    else
    {
        StackPush(params->doc_stack, data->val);
    }

    return PARSE_LINE;
}

int HandleOpenNote(StateMachineData* data)
{
    SParserParams *params;
    char stack_top = 0;
    params = data->params;

    if(!StackIsEmpty(params->doc_stack))
    {
        StackPeek(params->doc_stack, &stack_top);

        if(stack_top != '"' && params->line[params->line_index] == '*')
        {
            StackPush(params->doc_stack, &params->line[params->line_index]);
            ++params->line_index;
        }
    }

    return PARSE_LINE;
}   

int HandleCloseNote(StateMachineData* data)
{
    SParserParams *params;
    char stack_top = 0;
    char output = 0;

    params = data->params;

    if(!StackIsEmpty(params->doc_stack))
    {
        if(params->line[params->line_index] == '/') /* possible reach to end on note */
        {
            StackPeek(params->doc_stack, &stack_top);
            if(stack_top == '*') /* note was open previously in txt - close it. */
            {
                StackPop(params->doc_stack, &output);
            }
            ++params->line_index;
        }
    }

    return PARSE_LINE;
}

int PrintIsTextBalanced(StateMachineData* data)
{   
    SParserParams *parser_data;
    parser_data = (SParserParams *)data->params;

    /*PrintLineReport(parser_data->line, StackIsEmpty(parser_data->line_stack));*/
    
    printf("\n\n****** SUMMARY ******\n\n");
    printf("The input is ");

    if(!StackIsEmpty(parser_data->doc_stack))
    {
        printf("not ");
    }

    printf("balanced\n");

    return STATE_MACHINE_END_STATE;
}

int isParenthesMatch(char stackTop, char newInput)
{
    switch(stackTop)
    {
        case '(':
            return ')' == newInput;
        case '[':
            return ']' == newInput;
        case '{':
            return '}' == newInput;
        default:
            /*printf("no match\n");*/
            return 0;
    }
}

void PrintLineReport(char *line, int isBalanced)
{
    printf("\n%s\n", line);
    printf("This line is ");
    
    if(!isBalanced)
        printf("not ");    
    
    printf("balanced\n");
}
