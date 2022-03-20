#ifndef __BASIC_DEFS_H__
#define __BASIC_DEFS_H__

#define MAX_LINE_LENGTH 81 /* including \n and \0 at end of line */
#define MAX_LABEL_NAME 31
#define INDEX_LENGTH 5 
#define NUM_FUNCTIONS 16
#define NUM_INSTRUCTIONS 4
#define NUM_REGISTERS 16
#define TRUE 1
#define FALSE 0

#include "colors.h"

#define ERR(string) printf("%serror: %s%s\n", CLR_RED, string, CLR_RESET)
#define ERR_AT(string, line) printf("%serror: %s at: %s%d%s\n", CLR_RED, string, CLR_YEL, line, CLR_RESET)
#define WARNING_AT(string, line) printf("%swarning: %s at: %s%d%s\n", CLR_YEL, string, CLR_RED, line, CLR_RESET)

typedef struct 
{
    int opcode;
    int funct;
    int num_params;
    char *name;
} SFunctions;

typedef enum
{
    MOV = 1, /* must start from 1, 0 is FALSE val */
    CMP,
    ADD,
    SUB,
    LEA,
    CLR,
    NOT,
    INC,
    DEC,
    JMP,
    BNE,
    JSR,
    RED,
    PRN,
    RTS,
    STP,
    NUM_EFUNCS
} EFunc;

typedef enum
{
    INST_DATA = 1, /* must start from 1, 0 is FALSE val */
    INST_STRING,
    INST_EXTERN,
    INST_ENTRY
} EInstruction;

void InitGlobals();

#endif /* __BASIC_DEFS_H__ */