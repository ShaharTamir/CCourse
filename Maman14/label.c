#include <stdlib.h> /* malloc, free */
#include <string.h> /* strlen, strcpy */
#include "basic_defs.h"
#include "parser.h"
#include "label.h"

#define MEM_MOD 16

struct _SLabel
{
    int type; /* bit field */
    int blk_count;
    int base_address;
    int offset;
    char *name;
};

SLabel *LabelCreate(char *name)
{
    SLabel *new_lbl = NULL;

    new_lbl = malloc(sizeof(SLabel));
    
    if(new_lbl)
    {
        new_lbl->name = NULL;
        new_lbl->type = 0;
        new_lbl->blk_count = 0;
        new_lbl-> base_address = 0;
        new_lbl->offset = 0;

        if(name)
        {
            new_lbl->name = (char *)malloc(strlen(name) + 1); /* +1 for delimiter */
        
            if(new_lbl->name)
            {
                strcpy(new_lbl->name, name);
            }
        }
    }

    return new_lbl;
}

void LabelDestroy(SLabel *lbl)
{
    if(lbl)
    {
        if(lbl->name)
        {
            free(lbl->name);
            lbl->name = NULL;
        }

        free(lbl);
        lbl = NULL;
    }
}

ELabelType LabelInstructToLblType(int instruction)
{
    switch(instruction)
    {
        case INST_DATA:
        case INST_STRING:
            return LBL_DATA;
        case INST_EXTERN:
            return LBL_EXTERN;
        case INST_ENTRY:
            return LBL_ENTRY;
    }

    return FALSE;
}

void LabelSetMemAddress(SLabel *lbl, int mem_address)
{
    if(!lbl)
        return;

    mem_address += MEM_ADD_OFFSET;
    lbl->offset = mem_address % MEM_MOD;
    lbl->base_address = mem_address - lbl->offset;
}

void LabelSetNumEncodeblks(SLabel *lbl, int cost)
{
    if(!lbl)
        return;
    
    lbl->blk_count = cost;
}

void LabelSetName(SLabel *lbl, char *name)
{
    if(!lbl)
        return;

    if(lbl->name)
    {
        free(lbl->name);
        lbl->name = NULL;
    }

    lbl->name = (char *)malloc(strlen(name) + 1); /* +1 for delimiter */
        
    if(lbl->name)
    {
        strcpy(lbl->name, name);
    }
}

int LabelSetType(SLabel *lbl, ELabelType type)
{
    int ret_val = FALSE;

    if(lbl && type < NUM_LABEL_TYPES)
    {
        if(lbl->type == (1 << LBL_EXTERN) && LBL_EXTERN != type)
        {
            ERR("trying to add attributes to an external attributes");
        }
        else if(LBL_EXTERN == type && lbl->type > 0 && lbl->type != (1 << LBL_EXTERN))
        {
            ERR("trying to define .extern to an already defined symbol");
        }
        else if ((LBL_CODE == type && lbl->type & (1 << LBL_DATA)) ||
            (LBL_DATA == type && lbl->type & (1 << LBL_CODE)))
        {
            ERR("trying to define symbol with both .code and .data attributes");
        }
        else if((LBL_CODE == type && lbl->type & (1 << LBL_CODE)) ||
            (LBL_DATA == type && lbl->type & (1 << LBL_DATA)))
        {
            ERR("trying to define label twice with the same attributes");
        }
        else
        {
            ret_val = TRUE;
            lbl->type |= 1 << type;
        }
    }

    return ret_val;
}

char *LabelGetName(SLabel *lbl)
{
    return lbl->name;
}

int LabelGetBaseAddress(SLabel *lbl)
{
    return lbl->base_address;
}

int LabelGetOffset(SLabel *lbl)
{
    return lbl->offset;
}

int LabelIsEntry(SLabel *lbl)
{
    return lbl->type == (1 << LBL_ENTRY);
}

int LabelIsAlsoEntry(SLabel *lbl)
{
    return lbl->type & (1 << LBL_ENTRY);
}

int LabelIsExtern(SLabel *lbl)
{
    return lbl->type == (1 << LBL_EXTERN);
}

int LabelIsData(SLabel *lbl)
{
    return lbl->type & (1 << LBL_DATA);
}

int LabelCompareName(void *lbl, void *str, void *params)
{
    (void) params;

    return strcmp((char *)(((SLabel*)lbl)->name), (char *)str);
}

int LabelDestroyWrapper(void *data, void *params)
{
    SLabel *lbl = NULL;
    lbl = (SLabel *)data;

    LabelDestroy(lbl);

    return TRUE;
}

int LabelCalcDataMemAddress(void *data, void *params)
{
    SLabel *lbl = NULL;
    int *total_inst_count = NULL;

    lbl = (SLabel *)data;
    total_inst_count = (int *)params;

    if(LabelIsData(lbl))
    {
        LabelSetMemAddress(lbl, *total_inst_count + lbl->blk_count);
    }

    return TRUE;
}
