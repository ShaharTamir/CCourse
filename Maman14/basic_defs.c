#include "basic_defs.h"

const SFunctions g_func_names[NUM_FUNCTIONS] = { {0, 0, "mov"}, {1, 0, "cmp"}, {2, 10, "add"}, {2, 11, "sub"}, {4, 0, "lea"}, {5, 10, "clr"}, \
     {5, 11, "not"}, {5, 12, "inc"}, {5, 13, "dec"}, {9, 10, "jmp"}, {9, 11, "bne"}, {9, 12, "jsr"}, {12, 0, "red"}, \
     {13, 0, "prn"}, {14, 0, "rts"}, {15, 0, "stop"}};

const char *g_instructions[] = { ".data", ".string", ".extern", ".entry"};

const char *g_registers[NUM_REGISTERS] = { "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", \
                        "r10", "r11", "r12", "r13", "r14", "r15", "r16"};

const char g_special_chars[] = {';', ':', '#', '-', '+', ',', '"'};

void InitGlobals()
{
     (void) g_func_names;
     (void) g_instructions;
     (void) g_registers;
     (void) g_special_chars;
}