#ifndef __BASIC_DEFS_H__
#define __BASIC_DEFS_H__

#define MAX_LINE_LENGTH 82 /* including \n and \0 at end of line */
#define MAX_LABEL_NAME 31
#define TRUE 1
#define FALSE 0

#include <colors.h>

typedef struct 
{
    int opcode;
    int funct;
    char *name;
} SFunctions;

SFunctions *g_func_names[] = { {0, 0, "mov"}, {1, 0, "cmp"}, {2, 10, "add"}, {2, 11, "sub"}, {4, 0, "lea"}, {5, 10, "clr"}, \
     {5, 11, "not"}, {5, 12, "inc"}, {5, 13, "dec"}, {9, 10, "jmp"}, {9, 11, "bne"}, {9, 12, "jsr"}, {12, 0, "red"}, \
     {13, 0, "prn"}, {14, 0, "rts"}, {15, 0, "stop"}, {0, 0, "macro"} };

char *g_instructions[] = { ".data", ".string", ".extern", ".entry"};

char *g_registers[] = { "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", \
                        "r10", "r11", "r12", "r13", "r14", "r15", "r16"};

char g_special_chars[] = {';', ':', '#', '-', '+', ',', '"'};

typedef struct 
{
    int attributes;
    int base_address;
    int offset;
    char *name;
} Label;


#endif /* __BASIC_DEFS_H__ */