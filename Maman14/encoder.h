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

int InitEncoderData(SFileHandlerData *fh, SLinkedList *sym_table, \
    SEncoderData *en_data, const char *file_name, int open_ent, int open_ext);

void DestroyEncoderData(SEncoderData *en_data);

void EncodeObjHeadline(SEncoderData *en_data, int DC, int IC);

int EncodeLine(SEncoderData *ed);

#endif /* __ENCODER_H__ */