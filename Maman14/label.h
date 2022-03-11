#ifndef _LABEL_H_
#define _LABEL_H_

typedef struct _SLabel SLabel;

typedef enum
{
    EXTERN, /* 0001 */
    ENTRY,  /* 0010 */
    CODE,   /* 0100 */
    DATA,   /* 1000 */
    NUM_LABEL_TYPES
} ELabelType;

/* Creates a new label named 'name'. name can also be NULL. */
SLabel *LabelCreate(char *name);

/* Destroys label */
void LabelDestroy(SLabel *lbl);

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

#endif  /* _LABEL_H_ */ 