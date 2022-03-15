#define _GNU_SOURCE
#include <stdio.h> /* FILE, getline, fseek */
#include <ctype.h> /* isalpha, isspace */
#include <string.h> /* strlen */
#include <stdlib.h> /* malloc, free */

#include "basic_defs.h"
#include "parser.h"

#define DELIMITER '\0'
#define NOTE ';'
#define LABEL_DEF ':'
#define STRING_DEF '"'
#define POS '+'
#define NEG '-'
#define NUM_SEPARATOR ','
#define MAX_NUMBER 

extern const SFunctions g_func_names[NUM_FUNCTIONS];
extern const char *g_registers[NUM_REGISTERS];
extern const char *g_instructions[NUM_INSTRUCTIONS];

static int SkipSpaces(char *line, int curr_index, int line_len)
{
    /* skip all white spaces before word */
    while(curr_index < line_len && 
        isspace(line[curr_index]))
    {
        ++curr_index;
    }

    return curr_index;
}

static int ReverseSkipSpaces(char *line, int curr_index)
{
    while(curr_index >= 0 && 
        isspace(line[curr_index]))
    {
        --curr_index;
    }

    return curr_index;
}

void ParserMoveToLineNumber(FILE *input, int line_len, int line_number)
{
    size_t dummy = MAX_LINE_LENGTH;
    char *dummy_line = NULL;
    int i = 0;
    
    dummy_line = malloc(line_len);
    fseek(input, 0, SEEK_SET);
    
    for(i = 0; i < line_number; ++i)
    {
        getline(&dummy_line, &dummy, input);
    }

    free(dummy_line);
}

int ParserIsNewLabel(char *word)
{
    int length = 0;
    length = strlen(word);

    if(length > 1 && word[length - 1] == LABEL_DEF)
    {
        word[length - 1] = DELIMITER;
        return TRUE;
    }

    return FALSE;
}

int ParserIsExtEnt(char *word)
{
    if(!strcmp(word, g_instructions[INST_EXTERN - 1]))
        return INST_EXTERN;
    if(!strcmp(word, g_instructions[INST_ENTRY - 1]))
        return INST_ENTRY;
    
    return FALSE;
}

int ParserIsDataString(char *word)
{
    if(!strcmp(word, g_instructions[INST_DATA - 1]))
        return INST_DATA;
    if(!strcmp(word, g_instructions[INST_STRING - 1]))
        return INST_STRING;
    
    return FALSE;
}

int ParserIsLineNote(char *line, int line_len)
{
    int first_word_index = 0;

    first_word_index = SkipSpaces(line, first_word_index, line_len);

    return line[first_word_index] == NOTE || line[first_word_index] == DELIMITER;
}

int ParserIsFunction(char *word)
{
    int i = 0;

    for(i = 0; i < NUM_FUNCTIONS; ++i)
    {
        if(!strcmp(word, g_func_names[i].name))
        {
            return i + 1;
        }
    }

    return FALSE;
}

int ParserIsRegister(char *word)
{
    int i = 0;

    for(i = 0; i < NUM_REGISTERS; ++i)
    {
        if(!strcmp(word, g_registers[i]))
        {
            return i + 1;
        }
    }

    return FALSE;
}

int ParserNextWord(char *line, char *word, int curr_index, int line_len)
{
    int i = 0;

    curr_index = SkipSpaces(line, curr_index, line_len);

    while(curr_index < line_len 
        && !isspace(line[curr_index]))
    {
        word[i] = line[curr_index];
        ++i;
        ++curr_index;
    }

    word[i] = DELIMITER;

    return curr_index;
}

int ParserIsMoreWords(char *line, int curr_index, int line_len)
{
    curr_index = SkipSpaces(line, curr_index, line_len);

    return curr_index == line_len - 1;
}

int ParserValidateName(char *name)
{
    int len = 0;
    int i = 0;
    int ret_val = FALSE;

    len = strlen(name);

    if(len < MAX_LABEL_NAME && len > 0)
    {
        while((isalpha(name[i]) || isdigit(name[i])) && i < len)
        {
            ++i;
        }

        if(i == len)
        {
            ret_val = !ParserIsFunction(name) && !ParserIsRegister(name);

            if(!ret_val)
                ERR("name canno't be same as registers or functions");
        }
        else
        {
            ERR("name may conatin only ascii letters and digits");
        }
    }
    else
    {
        ERR("name length is not valid");
    }

    return ret_val;
}

int ParserIsValidString(char *line, int index, int line_len)
{
    int ret_val = FALSE;
    int end_string_index = 0;

    index = SkipSpaces(line, index, line_len);

    if(line[index] == STRING_DEF)
    {
        end_string_index = ReverseSkipSpaces(line, line_len - 1);

        if(end_string_index > index && line[end_string_index] == STRING_DEF)
        {
            ret_val = TRUE;
        }
    }

    return ret_val;
}

int ParserIsValidData(char *line, int index, int line_len)
{
    int ret_val = TRUE;
    int expect_num = FALSE;

    while(index < line_len) /* check string, delimiter ('\0') not included */
    {
        index = SkipSpaces(line, index, line_len);

        if(line[index] == POS || line[index] == NEG)
            ++index;

        if(isdigit(line[index]))
        {
            expect_num = FALSE;
            while(isdigit(line[index]))
            {
                ++index;
            }

            index = SkipSpaces(line, index, line_len);
            if(line[index] == NUM_SEPARATOR)
            {
                expect_num = TRUE;
                ++index;
            }
        }
        else
        {
            ret_val = FALSE;
            break;
        }
    }

    if(expect_num)
    {
        ERR("array end with num-separator, should end with number");
        ret_val = FALSE;
    }
    else if(!ret_val)
    {
        ERR("invalid data character sequence");
    }

    return ret_val;
}