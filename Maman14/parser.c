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
#define NUM_START '#'
#define SEPARATOR ','
#define OPEN_INDEX '['
#define CLOSE_INDEX ']'
#define ASCII_NUM_TO_INT 48
#define BASE_DEC 10
#define INDEX_DIG_OFFSET 2
#define LINE_LAST_NOTE_OFFSET 2

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

    if(length > 0 && word[length - 1] == LABEL_DEF)
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

int ParserIsIndex(char *word)
{
    int len = 0;

    len = strlen(word);

    return len > 0 && word[len - 1] == CLOSE_INDEX;
}

int ParserIsNumber(char *word)
{
    int i = 0;

    if(word[i] == NUM_START)
    {
        ++i;
        
        if(word[i] == POS || word[i] == NEG)
            ++i;

        while(isdigit(word[i]))
        {
            ++i;
        }
        
        if(word[i] == DELIMITER)
            return TRUE;
    }

    return FALSE;
}

int ParserSkipSeparator(char *line, int curr_index, int line_len)
{
    curr_index = SkipSpaces(line, curr_index, line_len);
    
    if(line[curr_index] == SEPARATOR)
        return curr_index + 1;

    return curr_index;
}

void ParserCleanSeparator(char *word)
{
    int len = 0;

    len = strlen(word);
    word[len - 1] = word[len - 1] == SEPARATOR ? DELIMITER : word[len - 1];
}

int ParserIndexNum(char *word)
{
    return BASE_DEC + (word[strlen(word) - INDEX_DIG_OFFSET] - ASCII_NUM_TO_INT);
}

int ParserExtractIndexFromWord(char *word, char *index_container)
{
    int len = 0;
    int i = 0;
    int j = 0;

    /* separate between name of label and register index */
    len = strlen(word);
    
    while(i < len && word[i] != OPEN_INDEX && /* collect name before first '[' */
        i < MAX_LABEL_NAME)
    {
        ++i;
    }

    if(i == len)
        return FALSE;
    
    word[i++] = DELIMITER;
    index_container[j++] = OPEN_INDEX;

    while(i < len && j < INDEX_LENGTH)
    {
        index_container[j] = word[i];
        ++j;
        ++i;
    }

    index_container[j] = DELIMITER;
    
    return i == len;
    /* if word is finished then the index length is right,
     and there are no other invalid chars after index */
}

int ParserNextWord(char *line, char *word, int curr_index, int line_len)
{
    int i = 0;

    curr_index = SkipSpaces(line, curr_index, line_len);

    while(curr_index < line_len && !isspace(line[curr_index]) 
        && line[curr_index] != SEPARATOR)
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

    return curr_index < line_len - 1;
}

int ParserCountSeparators(char *line, int curr_index, int line_len)
{
    int count = 0;

    for(; curr_index < line_len; ++curr_index)
    {
        if(line[curr_index] == SEPARATOR)
            ++count;
    }

    return count;
}

int ParserValidateLineLen(int line_len)
{
    return line_len < MAX_LINE_LENGTH;
}

int ParserValidateName(char *name)
{
    int len = 0;
    int i = 0;
    int ret_val = TRUE;

    len = strlen(name);

    if(len >= MAX_LABEL_NAME || len <= 0)
    {
        ERR("name length is not valid");
        ret_val = FALSE;
    }

    if(!(isalpha(name[0])))
    {
        ERR("name must start with alphabet letter");
        ret_val = FALSE;
    }

    while((isalpha(name[i]) || isdigit(name[i])) && i < len)
    {
        ++i;
    }

    if(i == len)
    {
        if(ret_val)
        {
            ret_val = !ParserIsFunction(name) && !ParserIsRegister(name);
            if(!ret_val)
                ERR("name canno't be same as registers or functions");
        }    
    }
    else
    {
        ERR("name may conatin only ascii letters and digits");
        ret_val = FALSE;
    }

    return ret_val;
}

int ParserValidateIndex(char *word)
{
    char valid_chars[] = {']', 'd', '1', 'r', '['}; /* 'd' is for dummy */
    char valid_reg[] = {'0', '1', '2', '3', '4', '5'};
    int end = 0;
    int count = 0;
    int i = 0;

    end = strlen(word);

    if(end != sizeof(valid_chars))
        return FALSE;

    --end; /* end is now last valid index in word */

    for(count = 0; count < sizeof(valid_chars); ++count)
    {
        if(count == 1) /* index of first digit (from end) */
        {
            for(i = 0; i < sizeof(valid_reg); ++i)
            {
                if(word[end - count] == valid_reg[i])
                    break;
            }

            if(i == sizeof(valid_reg))
                return FALSE;
        }
        else if(word[end - count] != valid_chars[count])
            return FALSE;
    }

    return TRUE;
}

int ParserIsValidString(char *line, int index, int line_len)
{
    int ret_val = FALSE;
    int end_string_index = 0;

    index = SkipSpaces(line, index, line_len);

    if(line[index] == STRING_DEF)
    {
        end_string_index = ReverseSkipSpaces(line, line_len - 1);

        if(end_string_index > index + 1 && /* string is not empty */
            line[end_string_index] == STRING_DEF) /* and closed */
        {
            ret_val = end_string_index - index;
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
            if(line[index] == SEPARATOR)
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