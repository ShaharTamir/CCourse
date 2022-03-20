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

/* calculate how many encoded lines are to add to total code instructions for each function */
static int CalcCodeLines(SFunctionHandlerData *fhd, int func);

/* convert access method to num of code instructions to encode */
static int ConvMethAccToBlks(int meth);

/* return access type if valid, else return FALSE */
static int GetIsValidAccessingMethod(char *word);

/* validate function with 1 parameter */
static int ValidateOneVariable(SFunctionHandlerData *fhd, int groups);

/* validate function with 2 parameters */
static int ValidateTwoVariable(SFunctionHandlerData *fhd, int groups_a, int groups_b);

/* functions that set bitfield according to groups using - Add AccessMethToGroup */
static int ValidateGroupOne(SFunctionHandlerData *fhd);
static int ValidateGroupTwo(SFunctionHandlerData *fhd);
static int ValidateGroupThree(SFunctionHandlerData *fhd);
static int ValidateGroupFour(SFunctionHandlerData *fhd);
static int ValidateGroupFive(SFunctionHandlerData *fhd);
static int ValidateGroupSix(SFunctionHandlerData *fhd);
static int ValidateGroupSeven(SFunctionHandlerData *fhd);

/************** API FUNCTIONS *************/
int FunctionValidateFunc(SFileHandlerData *fh, int *num_instructions, int func)
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

    *num_instructions += CalcCodeLines(&fhd, func);

    return ret_val;
}

EAccessMeth FunctionGetAccessingMethod(char *word)
{
    if(ParserIsNumber(word))
        return AC_IMMEDIATE;
    if(ParserIsRegister(word))
        return AC_REG;
    if(ParserIsIndex(word))
        return AC_INDEX;
    return AC_DIRECT;
}

/************** SERVICE FUNCTIONS *************/

int AddAccessMethToGroup(int group, EAccessMeth acc_meth)
{
    group |= 1 << acc_meth;

    return group;
}

int CalcCodeLines(SFunctionHandlerData *fhd, int func)
{
    int num_blks = 1; /* start with 1 for opcode line and 1 for funct */

    switch(func)
    {
        case MOV: case CMP: case LEA: /* 2 param functions */ 
        case ADD: case SUB: 
            ++num_blks; /* funct line */
            num_blks += ConvMethAccToBlks(fhd->acc_meth_a) + ConvMethAccToBlks(fhd->acc_meth_b);
            break;
        case CLR: case NOT: case INC: /* 1 param functions */
        case DEC: case JMP: case RED: 
        case PRN: case BNE: case JSR:
            ++num_blks; /* funct line */
            num_blks += ConvMethAccToBlks(fhd->acc_meth_b);
            break;
    }

    return num_blks;
}

int ConvMethAccToBlks(int meth)
{
    int sum = 0;

    switch(meth)
    {
        case AC_IMMEDIATE: /* value */
            sum += 1;
            break;
        case AC_DIRECT: /* base + offset */
        case AC_INDEX:
            sum += 2;
            break;
        case AC_REG: /* encoded in funct line */
            break;
    }

    return sum;
}

int GetIsValidAccessingMethod(char *word)
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
        acc_meth = GetIsValidAccessingMethod(fhd->fh->word);        
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
        acc_meth = GetIsValidAccessingMethod(fhd->fh->word);
        fhd->acc_meth_a = acc_meth;

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
