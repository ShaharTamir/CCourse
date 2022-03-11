#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdio.h> /* FILE */

/* moves to a specific line number in file (top line is 1?) */
void ParserMoveToLineNumber(FILE *input, int line_len, int line_number);

/* return TRUE if line is empty or note (means can discard the line) */
int ParserIsLineNote(char *line, int line_len);

/* return TRUE if next word is a function */
int ParserIsFunction(char *word);

/* extract next word from line. return the updated curr_index */
int ParserNextWord(char *line, char *word, int curr_index, int line_len);

/* 
verify name is not a function and that contains only alphabet characters.
if name is valid and contains ':' at end of name, this function removes it.
*/
int ParserValidateName(char *name);

#endif /* __PARSER_H__ */