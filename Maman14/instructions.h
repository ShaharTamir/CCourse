#ifndef __INSTRUCTIONS_H__
#define __INSTRUCTIONS_H__

/*
instructions group 1 - two operands
    mov(src, dest); // cpy src to dest
    cmp(src, dest); // Z=src-dest
    add(src, dest); // dest = src+dest
    sub(src, dest); // dest = src-dest
    lrs(src, dest); // put src address in dest
*/

/*
instructions group 2 - one operand

    clr(dest); // memset 0
    not(dest); // dest = ~dest
    inc(dest); // ++dest
    dec(dest); // --dest
    jmp(dest); // goto dest;
    bne(dest); // if Z is 0, jump to dest
    jsr(dest); // program stack pntr recieve new function pntr to goto next
    red(dest); // read one note from stdin into dest
    prn(dest); // print dest (one note) to stdout
*/

/*
instructions group 3 - no operands

    





*/

#endif /* __INSTRUCTIONS_H__ */