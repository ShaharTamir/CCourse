#include <string.h> /* strlen */

#include "basic_defs.h" /* EFunc */
#include "parser.h"
#include "function.h"


#define VALID_REG_INDEX_LEN 3

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

static int ValidateOneVariable(SFileHandlerData *fh, int groups);
static int ValidateTwoVariable(SFileHandlerData *fh, int groups_a, int groups_b);

static int ValidateGroupOne(SFileHandlerData *fh);
static int ValidateGroupTwo(SFileHandlerData *fh);
static int ValidateGroupThree(SFileHandlerData *fh);
static int ValidateGroupFour(SFileHandlerData *fh);
static int ValidateGroupFive(SFileHandlerData *fh);
static int ValidateGroupSix(SFileHandlerData *fh);
static int ValidateGroupSeven(SFileHandlerData *fh);

int FunctionValidateFunc(SFileHandlerData *fh, int func)
{
    int ret_val = FALSE;

    switch(func)
    {
        case CMP:
            ret_val = ValidateGroupOne(fh);
            break;
        case MOV:
        case ADD:
        case SUB:
            ret_val = ValidateGroupTwo(fh);
            break;
        case LEA:
            ret_val = ValidateGroupThree(fh);
            break;
        case PRN:
            ret_val = ValidateGroupFour(fh);
            break;
        case CLR:
        case NOT:
        case INC:
        case DEC:
        case RED:
            ret_val = ValidateGroupFive(fh);
            break;
        case JMP:
        case BNE:
        case JSR:
            ret_val = ValidateGroupSix(fh);
            break;
        case RTS:
        case STP:
            ret_val = ValidateGroupSeven(fh);
            break;
        default:
            ERR_AT("invalid function", fh->line_count);
    }

    return ret_val;
}

int AddAccessMethToGroup(int group, EAccessMeth acc_meth)
{
    group |= 1 << acc_meth;

    return group;
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

int ValidateOneVariable(SFileHandlerData *fh, int groups)
{
    int acc_meth = 0;

    if(ParserIsMoreWords(fh->line, fh->index, fh->bytes_read) && 
        !ParserCountSeparators(fh->line, fh->index, fh->bytes_read))
    {
        fh->index = ParserNextWord(fh->line, fh->word, fh->index, fh->bytes_read);
        acc_meth = GetAccessingMethod(fh->word);
        printf("access_meth group_b: %d\n", acc_meth);

        return !ParserIsMoreWords(fh->line, fh->index, fh->bytes_read) &&
            ((1 << acc_meth) & groups) != 0;
    }

    return FALSE;
}

int ValidateTwoVariable(SFileHandlerData *fh, int group_a, int group_b)
{
    int acc_meth = 0;

    if(ParserIsMoreWords(fh->line, fh->index, fh->bytes_read) && 
        1 == ParserCountSeparators(fh->line, fh->index, fh->bytes_read))
    {
        fh->index = ParserNextWord(fh->line, fh->word, fh->index, fh->bytes_read);
        ParserCleanSeparator(fh->word);
        acc_meth = GetAccessingMethod(fh->word);
        printf("access_meth group_a: %d\n", acc_meth);

        if(ParserIsMoreWords(fh->line, fh->index, fh->bytes_read) && 
            ((1 << acc_meth) & group_a) != 0)
        {
            fh->index = ParserSkipSeparator(fh->line, fh->index, fh->bytes_read);

            return ValidateOneVariable(fh, group_b);
        }
    }

    return FALSE;
}

int ValidateGroupOne(SFileHandlerData *fh)
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

    return ValidateTwoVariable(fh, group_a, group_b);
}

int ValidateGroupTwo(SFileHandlerData *fh)
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

    return ValidateTwoVariable(fh, group_a, group_b);
}

int ValidateGroupThree(SFileHandlerData *fh)
{
    int group_a = 0;
    int group_b = 0;

    group_a = AddAccessMethToGroup(group_a, AC_DIRECT);
    group_a = AddAccessMethToGroup(group_a, AC_INDEX);

    group_b = AddAccessMethToGroup(group_b, AC_DIRECT);
    group_b = AddAccessMethToGroup(group_b, AC_INDEX);
    group_b = AddAccessMethToGroup(group_b, AC_REG);

    return ValidateTwoVariable(fh, group_a, group_b);
}

int ValidateGroupFour(SFileHandlerData *fh)
{
    int group = 0;

    group = AddAccessMethToGroup(group, AC_IMMEDIATE);
    group = AddAccessMethToGroup(group, AC_DIRECT);
    group = AddAccessMethToGroup(group, AC_INDEX);
    group = AddAccessMethToGroup(group, AC_REG);
    
    return ValidateOneVariable(fh, group);
}

int ValidateGroupFive(SFileHandlerData *fh)
{
    int group = 0;

    group = AddAccessMethToGroup(group, AC_DIRECT);
    group = AddAccessMethToGroup(group, AC_INDEX);
    group = AddAccessMethToGroup(group, AC_REG);
    
    return ValidateOneVariable(fh, group);
}

int ValidateGroupSix(SFileHandlerData *fh)
{
    int group = 0;

    group = AddAccessMethToGroup(group, AC_DIRECT);
    group = AddAccessMethToGroup(group, AC_INDEX);
    
    return ValidateOneVariable(fh, group);
}

int ValidateGroupSeven(SFileHandlerData *fh)
{
    return !ParserIsMoreWords(fh->line, fh->index, fh->bytes_read);
}
