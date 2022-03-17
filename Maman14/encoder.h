#ifndef __ENCODER_H__
#define __ENCODER_H__

#include <stdio.h>
#include <linked_list.h>
#include "label.h"
#include "file_handler.h"

typedef struct
{
    int address;
    FILE *obj;
    FILE *ext;
    FILE *ent;
    SFileHandlerData *fh;
    SLinkedList *sym_tbl;
} SEncoderData;

void EncodeLabelToExtFile(FILE *out, char *lbl, int address);

void EncodeLblToEntryFile(FILE *out, SLabel *lbl);

void EncodeLineToObjectFile(FILE *out, int line, int address);

int InitEncoderData(SFileHandlerData *fh, SLinkedList *sym_table, SEncoderData *en_data, const char *file_name);

void DestroyEncoderData(SEncoderData *en_data);

int EncodeLine(SEncoderData *ed);

#endif /* __ENCODER_H__ */