#include "basic_defs.h"

const SFunctions g_func_names[NUM_FUNCTIONS] = { {0, 0, 2, "mov"}, {1, 0, 2, "cmp"}, {2, 10, 2, "add"}, {2, 11, 2, "sub"}, {4, 0, 2, "lea"}, {5, 10, 1, "clr"}, \
     {5, 11, 1, "not"}, {5, 12, 1, "inc"}, {5, 13, 1, "dec"}, {9, 10, 1, "jmp"}, {9, 11, 1, "bne"}, {9, 12, 1, "jsr"}, {12, 0, 1, "red"}, \
     {13, 0, 1, "prn"}, {14, 0, 0, "rts"}, {15, 0, 0, "stop"}};

const char *g_instructions[NUM_INSTRUCTIONS] = { ".data", ".string", ".extern", ".entry"};

const char *g_registers[NUM_REGISTERS] = { "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", \
                        "r10", "r11", "r12", "r13", "r14", "r15", "r16"};

void InitGlobals()
{
     (void) g_func_names;
     (void) g_instructions;
     (void) g_registers;
}