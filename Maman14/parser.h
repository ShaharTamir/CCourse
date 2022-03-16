#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdio.h> /* FILE */

/* moves to a specific line number in file (top line is 1?) */
void ParserMoveToLineNumber(FILE *input, int line_len, int line_number);

/* 
   return TRUE if next word is a new label definition.
   If TRUE - the string is already set without ':' to use to create a new label.
*/
int ParserIsNewLabel(char *word);

/*
   return EInstruction value if word is ".entry" or ".extern" (matching index of g_instructions + 1).
   else return FALSE.
*/
int ParserIsExtEnt(char *word);

/*
   return EInstruction value if word is ".data" or ".string" (matching index of g_instructions + 1).
   else return FALSE.
*/
int ParserIsDataString(char *word);

/* return TRUE if line is empty or note (means can discard the line) */
int ParserIsLineNote(char *line, int line_len);

/* 
   return EFunc value (from basic_defs) if next word is a function (matching index of g_functions + 1). 
   else return FALSE.
*/
int ParserIsFunction(char *word);

/* return register index + 1 if next word is a register */
int ParserIsRegister(char *word);

/* return TRUE if word ends with index bracket: ']' */
int ParserIsIndex(char *word);

/* return if word is a direct number - #num (example: #-7) */
int ParserIsNumber(char *word);

/* if found separator - return the next char index.
   else - return char index of the next word */
int ParserSkipSeparator(char *line, int curr_index, int line_len);

/* cleans separator (if exist) from end of word */
void ParserCleanSeparator(char *word);

/* split word into name and index words index word is saved into index_container.
   If name or index is longer then valid - the function return false.
   It is user responsibility to provide a valid index_container.
*/
int ParserExtractIndexFromWord(char *word, char *index_container);

/* extract next word from line. return the updated curr_index */
int ParserNextWord(char *line, char *word, int curr_index, int line_len);

/* return TRUE if there are more words after curr index */
int ParserIsMoreWords(char *line, int curr_index, int line_len);

/* return number of separators to count num of parameters to expect */
int ParserCountSeparators(char *line, int curr_index, int line_len);

/* verify name is not a function and that contains only alphabet characters. */
int ParserValidateName(char *name);

/* verify only the end of word is valid index access "[r1X]", X == [0-5]*/
int ParserValidateIndex(char *word);

/* verify string is valid - if true return string length */
int ParserIsValidString(char *line, int index, int line_len);

/* 
   verify data (numbers) is valid - number is valid (15bits limit).
   works for single numbers and arrays.
*/
int ParserIsValidData(char *line, int index, int line_len);

#endif /* __PARSER_H__ */