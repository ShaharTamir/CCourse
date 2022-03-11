#ifndef __PARSER_H__
#define __PARSER_H__

#define PARSER_FAIL 0
#define PARSER_SUCCESS 1

#include <stdio.h> /* FILE type */
#include <state_machine.h>

/* This function should parse the next line.
    Return value should be the next state for the parser state machine. */   
typedef int(*ParseFunction)(StateMachineData*);

typedef struct _SParser SParser;

/*
    These are the parser basic states.
    If one wants to add more states - 
    need to use ParserAddState function 
    #! and use indexs >= DEFAULT_NUM_STATES !#
    If override the basic parser states - you lose the parser suggested here.

    If PARSE_LINE reached to end of file (EOF) the file would go to FINISH_READ.
    To finish parser work before EOF - goto state FINISH_READ.
*/
typedef enum 
{
    READ_NEW_LINE,
    PARSE_LINE,
    FINISH_READ,
    DEFAULT_NUM_STATES
} EParserState;

/*
    Each handler of the parser states receive machine_data
    as a parameter. (from the state machine).
    machine_data->params are the parser_params here and can be accessed from
    all handlers.

    For any additional data please use the "user_data" field and send the "data"
    parameter at the ParserCreate.
*/
typedef struct
{
    int bytes_read;
    int line_count;
    int line_index;
    size_t line_len;
    void *user_data;
    FILE *input;
    char *line;
    ParseFunction line_parse_func;
} SParserParams;

/* ParserCreate:
 *  This function creates a new parser for the user to use.
 * 
 * params:
 *  input - the file descriptor ptr to read from. the file should be open for reading.
 *  data - user data that would be passed between the different state handlers.
 *  line_parser - basic line parser that should decide what to do with each line and send to different states.
 *  num_states - number of states (and handlers) the parser would use (including those about to be added by user).
 *  max_line_len - maximum length of each line read from file.
 *  
 * return:
 *  a new parser pointer initialized with all basic states.
*/
SParser* ParserCreate(FILE *input, void *data, \
    ParseFunction line_parser, int num_states, size_t max_line_len);

/* ParserDestroy:
 *  This function release all the memory parser allocated.
 * 
 * params:
 *  parser - parser to de-allocate.
 * 
 * return:
 *  none.
*/
void ParserDestroy(SParser *parser);

/* ParserAddState:
 *  This function adds a state and its handler to the parser state machine.
 *  Remember to run the parser state machine carefully.
 *  Again, to end the parser work - go to FINISH_READ state.
 * 
 * params:
 *  parser - to add state to.
 *  state_index - must be >= DEFAULT_NUM_STATES, or override the basic parser states if wish
 *              to implement the parser differnetly. (good luck with that..)
 *  handler - handle function for the state. should return the next wanted state (see state_machine.h).
 * 
 * return:
 *  PARSER_FAIL or PARSER_SUCCESS according to the process status.
*/
int ParserAddState(SParser *parser, int state_index, StateHandler handler);

/* ParserSetLineParseFunc:
 *  This functions sets the parsing function conducted over each line
 *  during the parsing process.
 *  
 * params:
 *  parser - to set its function.
 *  new_line_function - ...
 * 
 * return:
 *  None.
*/
void ParserSetLineParseFunc(SParser *parser, ParseFunction new_line_func);


char *ParserNextWord(SParserParams *data);

void ParserMoveToLineNumber(SParserParams* params, int line_number);

/* ParserRun:
 *  This function runs the parser state machine. It will stop when reaching the FINISH_READ state
 *  or if sent STATE_MACHINE_END_STATE state (see state_machine.h).
 *  Use machine_data to send any return value from the state machine.
 *  #! Notice that the parser is not destroyed automatically. !#
 * 
 * params:
 *  parser - the parser to run.
 * 
 * return:
 *  If the parser finished its work properly - PARSER_SUCCESS.
 *  Otherwise PARSER_FAIL.

return PARSER_FAIL / PARSER_SUCCESS */
int ParserRun(SParser *parser);

#endif /* __PARSER_H__ */
