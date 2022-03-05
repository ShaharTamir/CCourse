#include <stdlib.h> /* malloc, free */

#include "basic_defs.h"
#include "parser.h"
#include "label.h"

#define MEM_MOD 16

struct _SLabel
{
    int type; /* bit field */
    int base_address;
    int offset;
    char *name;
};

SLabel *LabelCreate(char *name)
{
    SLabel *new_lbl = NULL;
    int name_len = 0;

    new_lbl = malloc(sizeof(SLabel));
    
    if(new_lbl)
    {
        new_lbl->name = NULL;
        new_lbl->type = 0;
        new_lbl-> base_address = 0;
        new_lbl->offset = 0;

        if(name)
        {
            new_lbl->name = (char *)malloc(strlen(name) + 1);
        
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

void LabelSetMemAddress(SLabel *lbl, int mem_address)
{
    if(!lbl)
        return;

    lbl->offset = mem_address % MEM_MOD;
    lbl->base_address = mem_address - lbl->offset;
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

    lbl->name = (char *)malloc(strlen(name) + 1);
        
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
        if(lbl->type & (1 << EXTERN))
        {
            printf("%serror: trying to add attributes to an external attributes%s", CLR_RED, CLR_WHT);
        }
        else if(EXTERN == type && lbl->type != 0)
        {
            printf("%serror: trying to define .extern to an already defined symbol%s", CLR_RED, CLR_WHT);
        }
        else if ((CODE == type && lbl->type & (1 << DATA)) ||
            (DATA == type && lbl->type & (1 << CODE)))
        {
            printf("%serror: trying to define symbol with both .code and .data attributes%s", CLR_RED, CLR_WHT);
        }
        else
        {
            ret_val = TRUE;
            lbl->type |= 1 << type;
        }
    }

    return ret_val;
}

