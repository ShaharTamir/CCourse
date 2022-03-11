#ifndef __BASIC_DEFS_H__
#define __BASIC_DEFS_H__

#define MAX_LINE_LENGTH 81 /* including \n and \0 at end of line */
#define MAX_LABEL_NAME 31
#define NUM_FUNCTIONS 16
#define NUM_INSTRUCTIONS 4
#define NUM_REGISTERS 16
#define NUM_SPECIAL_CHARS 7
#define TRUE 1
#define FALSE 0

#include <colors.h>

typedef struct 
{
    int opcode;
    int funct;
    char *name;
} SFunctions;

typedef enum
{
    MOV,
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
    STOP,
    NUM_EFUNCS
} EFunc;

void InitGlobals();

#endif /* __BASIC_DEFS_H__ */