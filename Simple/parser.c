#include <stdio.h> /* FILE, getline, fseek */
#include <ctype.h> /* isalpha, isspace */
#include <string.h> /* strlen */
#include <stdlib.h> /* malloc, free */

#include "basic_defs.h"
#include "parser.h"

#define DELIMITER '\0'
#define NOTE ';'
#define LABEL_DEF ':'

extern g_func_names;

static int SkipSpaces(char *line, int curr_index, int line_len)
{
    /* skip all white spaces before word */
    while(curr_index < line_len 
        && isspace(line[curr_index]))
    {
        ++curr_index;
    }

    return curr_index;
}

void ParserMoveToLineNumber(FILE *input, int line_len, int line_number)
{
    size_t dummy;
    char *dummy_line;
    int i = 0;
    
    dummy_line = malloc(line_len);
    
    fseek(input, 0, SEEK_SET);
    for(i = 0; i < line_number; ++i)
    {
        getline(&dummy_line, &dummy, input);
    }

    free(dummy_line);
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
    int num_functions = 0;

    num_functions = sizeof(g_func_names) / sizeof(SFunctions);

    for(i = 0; i < num_functions; ++i)
    {
        if(!strcmp(word, g_func_names[i].name))
        {
            return TRUE;
        }
    }

    return FALSE;
}

int ParserIsRegister(char *word)
{
    int i = 0;
    int num_registers = 0;

    num_registers = sizeof(g_registers) / sizeof(char *);

    for(i = 0; i < num_registers; ++i)
    {
        if(!strcmp(word, g_registers[i]))
        {
            return TRUE;
        }
    }

    return FALSE;
}

void ParserNextWord(char *line, char *word, int curr_index, int line_len)
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
}

int ParserValidateName(char *name)
{
    int len = 0;
    int valid_len = 0;
    int i = 0;
    int ret_val = FALSE;

    len = strlen(name);

    if(len < MAX_LABEL_NAME)
    {
        valid_len = name[len - 1] == LABEL_DEF ? len - 1 : len;

        while(isalpha(name[i]) && i < len)
        {
            ++i;
        }

        if(i == valid_len)
        {
            name[i] = DELIMITER;
            ret_val = !ParserIsFunction(name) && !ParserIsRegister(name);
        }
    }

    return ret_val;
}