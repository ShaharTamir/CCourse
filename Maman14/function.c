#include <string.h> /* strlen */

#include "basic_defs.h" /* EFunc */
#include "parser.h"
#include "function.h"

typedef struct
{
    int acc_meth_a;
    int acc_meth_b;
    SFileHandlerData *fh;
}SFunctionHandlerData;

/*****************************************************************
 *                      FUNCTION GROUPS
 *  according to accessing method numbers from instructions
 *  with offset of 1 (0 is saved for NULL) 
 *  src             dest            group num
 * 
 *  1,2,3,4         1,2,3,4         1
 *  1,2,3,4         2,3,4           2
 *  2,3             2,3,4           3
 *  NULL            1,2,3,4         4
 *  NULL            2,3,4           5
 *  NULL            2,3             6
 *  NULL            NULL            7
 ****************************************************************/

/* add to group the bit of acc_meth (group is bit field */
static int AddAccessMethToGroup(int group, EAccessMeth acc_meth);
static int CalcCodeLines(SFunctionHandlerData *fhd, int func);
static int ConvMethAccToBlks(int meth, int is_funct);

static int ValidateOneVariable(SFunctionHandlerData *fhd, int groups);
static int ValidateTwoVariable(SFunctionHandlerData *fhd, int groups_a, int groups_b);

static int ValidateGroupOne(SFunctionHandlerData *fhd);
static int ValidateGroupTwo(SFunctionHandlerData *fhd);
static int ValidateGroupThree(SFunctionHandlerData *fhd);
static int ValidateGroupFour(SFunctionHandlerData *fhd);
static int ValidateGroupFive(SFunctionHandlerData *fhd);
static int ValidateGroupSix(SFunctionHandlerData *fhd);
static int ValidateGroupSeven(SFunctionHandlerData *fhd);

int FunctionValidateFunc(SFileHandlerData *fh, int *num_encode_blocks, int func)
{
    SFunctionHandlerData fhd;
    int ret_val = FALSE;

    fhd.acc_meth_a = 0;
    fhd.acc_meth_b = 0;
    fhd.fh = fh;

    switch(func)
    {
        case CMP:
            ret_val = ValidateGroupOne(&fhd);
            break;
        case MOV: case ADD:  case SUB:
            ret_val = ValidateGroupTwo(&fhd);
            break;
        case LEA:
            ret_val = ValidateGroupThree(&fhd);
            break;
        case PRN:
            ret_val = ValidateGroupFour(&fhd);
            break;
        case CLR: case NOT: case INC: case DEC: case RED:
            ret_val = ValidateGroupFive(&fhd);
            break;
        case JMP: case BNE: case JSR:
            ret_val = ValidateGroupSix(&fhd);
            break;
        case RTS: case STP:
            ret_val = ValidateGroupSeven(&fhd);
            break;
        default:
            ERR_AT("invalid function", fh->line_count);
    }

    *num_encode_blocks += CalcCodeLines(&fhd, func);

    return ret_val;
}

int AddAccessMethToGroup(int group, EAccessMeth acc_meth)
{
    group |= 1 << acc_meth;

    return group;
}

int CalcCodeLines(SFunctionHandlerData *fhd, int func)
{
    int num_blks = 1; /* start with 1 for opcode line */

    switch(func)
    {
        case MOV: case CMP: case LEA: /* 2 param no funct code */
            num_blks += ConvMethAccToBlks(fhd->acc_meth_a, FALSE) + ConvMethAccToBlks(fhd->acc_meth_b, FALSE);
            break;
        case RED: case PRN: /* 1 param no funct code */
            num_blks += ConvMethAccToBlks(fhd->acc_meth_b, FALSE);
            break;
        case ADD: case SUB: /* 2 param funct code */
            ++num_blks;
            num_blks += ConvMethAccToBlks(fhd->acc_meth_a, TRUE) + ConvMethAccToBlks(fhd->acc_meth_b, TRUE); 
            break;
        case CLR: case NOT: case INC: case DEC: case JMP: /* 1 param funct code */
        case BNE: case JSR:
            ++num_blks;
            num_blks += ConvMethAccToBlks(fhd->acc_meth_b, TRUE);
            break;
    }

    return num_blks;
}

int ConvMethAccToBlks(int meth, int is_funct)
{
    int sum = 0;

    switch(meth)
    {
        case AC_IMMEDIATE:
        case AC_DIRECT:
            sum += 2;
            break;
        case AC_INDEX:
            sum += 3 - is_funct;
            break;
        case AC_REG:
            sum += 1 - is_funct;
            break;
    }

    return sum;
}

int GetAccessingMethod(char *word)
{
    char index_word[MAX_LABEL_NAME] = {0};

    if(ParserIsNumber(word))
        return AC_IMMEDIATE;
    if(ParserIsRegister(word))
        return AC_REG;
    if(ParserIsIndex(word))
    {
        if(ParserExtractIndexFromWord(word, index_word))
        {
            if(ParserValidateName(word) && ParserValidateIndex(index_word))
                return AC_INDEX;
        }
    }
    else if(ParserValidateName(word))
        return AC_DIRECT;
    
    return FALSE;
}

int ValidateOneVariable(SFunctionHandlerData *fhd, int groups)
{
    int acc_meth = 0;

    if(ParserIsMoreWords(fhd->fh->line, fhd->fh->index, fhd->fh->bytes_read) && 
        !ParserCountSeparators(fhd->fh->line, fhd->fh->index, fhd->fh->bytes_read))
    {
        fhd->fh->index = ParserNextWord(fhd->fh->line, fhd->fh->word, fhd->fh->index, fhd->fh->bytes_read);
        acc_meth = GetAccessingMethod(fhd->fh->word);        
        fhd->acc_meth_b = acc_meth;
        
        return !ParserIsMoreWords(fhd->fh->line, fhd->fh->index, fhd->fh->bytes_read) &&
            ((1 << acc_meth) & groups) != 0;
    }

    return FALSE;
}

int ValidateTwoVariable(SFunctionHandlerData *fhd, int group_a, int group_b)
{
    int acc_meth = 0;

    if(ParserIsMoreWords(fhd->fh->line, fhd->fh->index, fhd->fh->bytes_read) && 
        1 == ParserCountSeparators(fhd->fh->line, fhd->fh->index, fhd->fh->bytes_read))
    {
        fhd->fh->index = ParserNextWord(fhd->fh->line, fhd->fh->word, fhd->fh->index, fhd->fh->bytes_read);
        ParserCleanSeparator(fhd->fh->word);
        acc_meth = GetAccessingMethod(fhd->fh->word);
        fhd->acc_meth_a = acc_meth;
        /*printf("access_meth group_a: %d\n", acc_meth);*/

        if(ParserIsMoreWords(fhd->fh->line, fhd->fh->index, fhd->fh->bytes_read) && 
            ((1 << acc_meth) & group_a) != 0)
        {
            fhd->fh->index = ParserSkipSeparator(fhd->fh->line, fhd->fh->index, fhd->fh->bytes_read);
            return ValidateOneVariable(fhd, group_b);
        }
    }

    return FALSE;
}

int ValidateGroupOne(SFunctionHandlerData *fhd)
{
    int group_a = 0;
    int group_b = 0;

    group_a = AddAccessMethToGroup(group_a, AC_IMMEDIATE);
    group_a = AddAccessMethToGroup(group_a, AC_DIRECT);
    group_a = AddAccessMethToGroup(group_a, AC_INDEX);
    group_a = AddAccessMethToGroup(group_a, AC_REG);

    group_b = AddAccessMethToGroup(group_b, AC_IMMEDIATE);
    group_b = AddAccessMethToGroup(group_b, AC_DIRECT);
    group_b = AddAccessMethToGroup(group_b, AC_INDEX);
    group_b = AddAccessMethToGroup(group_b, AC_REG);

    return ValidateTwoVariable(fhd, group_a, group_b);
}

int ValidateGroupTwo(SFunctionHandlerData *fhd)
{
    int group_a = 0;
    int group_b = 0;

    group_a = AddAccessMethToGroup(group_a, AC_IMMEDIATE);
    group_a = AddAccessMethToGroup(group_a, AC_DIRECT);
    group_a = AddAccessMethToGroup(group_a, AC_INDEX);
    group_a = AddAccessMethToGroup(group_a, AC_REG);

    group_b = AddAccessMethToGroup(group_b, AC_DIRECT);
    group_b = AddAccessMethToGroup(group_b, AC_INDEX);
    group_b = AddAccessMethToGroup(group_b, AC_REG);

    return ValidateTwoVariable(fhd, group_a, group_b);
}

int ValidateGroupThree(SFunctionHandlerData *fhd)
{
    int group_a = 0;
    int group_b = 0;

    group_a = AddAccessMethToGroup(group_a, AC_DIRECT);
    group_a = AddAccessMethToGroup(group_a, AC_INDEX);

    group_b = AddAccessMethToGroup(group_b, AC_DIRECT);
    group_b = AddAccessMethToGroup(group_b, AC_INDEX);
    group_b = AddAccessMethToGroup(group_b, AC_REG);

    return ValidateTwoVariable(fhd, group_a, group_b);
}

int ValidateGroupFour(SFunctionHandlerData *fhd)
{
    int group = 0;

    group = AddAccessMethToGroup(group, AC_IMMEDIATE);
    group = AddAccessMethToGroup(group, AC_DIRECT);
    group = AddAccessMethToGroup(group, AC_INDEX);
    group = AddAccessMethToGroup(group, AC_REG);
    
    return ValidateOneVariable(fhd, group);
}

int ValidateGroupFive(SFunctionHandlerData *fhd)
{
    int group = 0;

    group = AddAccessMethToGroup(group, AC_DIRECT);
    group = AddAccessMethToGroup(group, AC_INDEX);
    group = AddAccessMethToGroup(group, AC_REG);
    
    return ValidateOneVariable(fhd, group);
}

int ValidateGroupSix(SFunctionHandlerData *fhd)
{
    int group = 0;

    group = AddAccessMethToGroup(group, AC_DIRECT);
    group = AddAccessMethToGroup(group, AC_INDEX);
    
    return ValidateOneVariable(fhd, group);
}

int ValidateGroupSeven(SFunctionHandlerData *fhd)
{
    return !ParserIsMoreWords(fhd->fh->line, fhd->fh->index, fhd->fh->bytes_read);
}
