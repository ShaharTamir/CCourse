#ifndef __ENCODER_H__
#define __ENCODER_H__

#include <stdio.h>
#include "linked_list.h"
#include "label.h"
#include "file_handler.h"

typedef struct
{
    int is_data_encode;
    int address;
    FILE *obj;
    FILE *ext;
    FILE *ent;
    SFileHandlerData *fh;
    SLinkedList *sym_tbl;
} SEncoderData;

/* init encoder struct. If success return TRUE, else - FALSE */
int InitEncoderData(SFileHandlerData *fh, SLinkedList *sym_table, \
    SEncoderData *en_data, const char *file_name, int open_ent, int open_ext);

/* close open files and remove all data from 'en_data' */
void DestroyEncoderData(SEncoderData *en_data);

/* write to object file the DC and IC */
void EncodeObjHeadline(SEncoderData *en_data, int DC, int IC);

/*
 * Write all line symbols to their relevant files. 
 * Verify all function variable symbols are defined. 
 * 
 * return TRUE if all symbols are defined.
 * else return FALSE.
*/
int EncodeLine(SEncoderData *ed);

#endif /* __ENCODER_H__ */