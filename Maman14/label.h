#ifndef _LABEL_H_
#define _LABEL_H_

typedef struct _SLabel SLabel;

typedef enum
{
    LBL_EXTERN, /* 0001 */
    LBL_ENTRY,  /* 0010 */
    LBL_CODE,   /* 0100 */
    LBL_DATA,   /* 1000 */
    NUM_LABEL_TYPES
} ELabelType;

/* Creates a new label named 'name'. name can also be NULL. */
SLabel *LabelCreate(char *name);

/* Destroys label */
void LabelDestroy(SLabel *lbl);

ELabelType LabelInstructToLblType(int instruction);

/* Set the memory address of label in program */
void LabelSetMemAddress(SLabel *lbl, int mem_address);

/* Set/replace name of a label */
void LabelSetName(SLabel *lbl, char *name);

/* 
    Set/add type to label type
    possible combinations:
        EXTERN,
        CODE,
        DATA,
        CODE + ENTRY
        DATA + ENTRY

    return TRUE if success, else FALSE
 */
int LabelSetType(SLabel *lbl, ELabelType type);

/* compare function for data structures (linked_list) */
int LabelCompareName(void *lbl, void *str, void *params);

/* destroy wrapper for data structures (linked_list) */
int LabelDestroyWrapper(void *data, void *params);

#endif  /* _LABEL_H_ */ 