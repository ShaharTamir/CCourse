#include <stdio.h> /* fprintf */
#include <stdlib.h> /* free */
#include <string.h> /* memset */

#include "basic_defs.h"
#include "label.h"
#include "file_handler.h"
#include "linked_list.h"
#include "parser.h"
#include "encoder.h"

#define START_ADD 100
#define FOUR_DIG 1000
#define DEC_BASE 10
#define NUM_NIBBLES 5
#define NIBBLE_SIZE 4
#define INST_NIBBLE 4
#define STRING_CLOSERS 2
#define MAX_NUM 32767
#define MIN_NUM -32768

typedef enum
{
    CODE_ABSOLUTE = 4,      /* 0100 */
    CODE_RELOCATABLE = 2,   /* 0010 */
    CODE_EXTERNAL = 1       /* 0001 */
} EInstType;

static int ExtractNibble(int line, int nibble_num);
static void SetCodeInstruction(int *line, int type);
static void SetNum(int *line, int num);
static void ClearLine(int *line);

void EncodeString(SEncoderData *ed);
void EncodeData(SEncoderData *ed);

void EncodeLblToExtFile(FILE *out, char *lbl_name, int address)
{
    char base[] = {" BASE "};
    char offset[] = {" OFFSET "};

    fprintf(out, "%s%s%d\n", lbl_name, base, address);
    fprintf(out, "%s%s%d\n\n", lbl_name, offset, address + 1);
}

void EncodeLblToEntryFile(FILE *out, SLabel *lbl)
{
    fprintf(out, "%s,%d,%d\n", LabelGetName(lbl), LabelGetBaseAddress(lbl), LabelGetOffset(lbl));
}

void EncodeLineToObjectFile(FILE *out, int line, int address)
{
    char nibble_part[] = {'E','D','C','B','A'};
    int i = 0;

    if(address < FOUR_DIG)
        fprintf(out, "0%d\t", address);
    else
        fprintf(out, "%d\t", address);

    for(i = NIBBLE_SIZE; i > 0; --i)
    {
        fprintf(out, "%c%X-", nibble_part[i], ExtractNibble(line, i));
    }

    fprintf(out, "%c%X\n", nibble_part[i], ExtractNibble(line, i));
}
/*
void EncodeLblToObj(FILE *out, SLabel *lbl, int address)
{
    int line = 0;

    if(LabelIsExtern(lbl))
    {
        SetCodeInstruction(&line, CODE_EXTERNAL);
        EncodeLineToObjectFile(out, line, address);
        EncodeLineToObjectFile(out, line, address + 1);
    }
    else
    {
        SetCodeInstruction(&line, CODE_RELOCATABLE);
        line += LabelGetBaseAddress(lbl);
        EncodeLineToObjectFile(out, line, address);
        line = line - LabelGetBaseAddress(lbl) + LabelGetOffset(lbl);
        EncodeLineToObjectFile(out, line, address + 1);
    }
}
*/

int InitEncoderData(SFileHandlerData *fh, SLinkedList *sym_table, SEncoderData *en_data, const char *file_name)
{
    char *new_file_name = NULL;

    new_file_name = FileHandlerGetFileName(file_name, STAGE_OBJ);
    en_data->obj = FileHandlerOpenFile(new_file_name, "w");
    free(new_file_name);

    if(en_data->obj)
    {
        new_file_name = FileHandlerGetFileName(file_name, STAGE_EXT);
        en_data->ext = FileHandlerOpenFile(new_file_name, "w");
        free(new_file_name);

        if(en_data->ext)
        {
            new_file_name = FileHandlerGetFileName(file_name, STAGE_ENT);
            en_data->ent = FileHandlerOpenFile(new_file_name, "w");
            free(new_file_name);

            if(en_data->ent)
            {
                en_data->address = START_ADD;
                en_data->fh = fh;
                en_data->sym_tbl = sym_table;

                return TRUE;
            }
        }
    }

    return FALSE;
}

void DestroyEncoderData(SEncoderData *en_data)
{
    if(en_data->obj)
    {
        fclose(en_data->obj);
    }
    if(en_data->ext)
    {
        fclose(en_data->ext);
    }
    if(en_data->ent)
    {
        fclose(en_data->ent);
    }

    memset(en_data, 0, sizeof(SEncoderData)); /* all is NULL / 0 */
}

int EncodeLine(SEncoderData *ed)
{
    int instruction = 0;

    ed->fh->index = ParserNextWord(ed->fh->line, ed->fh->word, ed->fh->index, ed->fh->bytes_read);

    if(!ParserIsExtEnt(ed->fh->word)) /* skip .entry and .extern lines */
    {
        if(ParserIsNewLabel(ed->fh->word)) /* no encode for label def */
        {
            ed->fh->index = ParserNextWord(ed->fh->line, ed->fh->word, ed->fh->index, ed->fh->bytes_read);
        }

        instruction = ParserIsDataString(ed->fh->word);

        if(instruction)
        {
            if(INST_STRING == instruction)
                EncodeString(ed);
            else
                EncodeData(ed);
        }
        else /* code */
        {
            /*return EncodeFunction(ed);*/
        }
    }

    return TRUE;
}

void EncodeString(SEncoderData *ed)
{
    int i = 0;
    int len = 0;
    int line = 0;

    /* word is now the string */
    ed->fh->index = ParserNextWord(ed->fh->line, ed->fh->word, ed->fh->index, ed->fh->bytes_read);
    len = strlen(ed->fh->word) - STRING_CLOSERS;
    SetCodeInstruction(&line, CODE_ABSOLUTE);

    for(i = 1; i <= len; ++i)
    {
        line += (int)(ed->fh->word[i]); /* char is also int */
        EncodeLineToObjectFile(ed->obj, line, ed->address);
        line -= (int)(ed->fh->word[i]); /* reset line for next char */
        ++ed->address;
    }
}

void EncodeData(SEncoderData *ed)
{
    int i = 0;
    int num_vars = 0;
    int num = 0;
    int line = 0;
    char *next_data = NULL;

    num_vars = ParserCountSeparators(ed->fh->line, ed->fh->index, ed->fh->bytes_read) + 1; /* count variables */
    ed->fh->index = ParserNextWord(ed->fh->line, ed->fh->word, ed->fh->index, ed->fh->bytes_read);
    SetCodeInstruction(&line, CODE_ABSOLUTE);

    for(i = 0; i < num_vars; ++i)
    {
        num = (int)strtol(ed->fh->word, &next_data, DEC_BASE);
        if(num > MAX_NUM || num < MIN_NUM)
            WARNING_AT("number out of size range, behaviour undefined", ed->fh->line_count);

        SetNum(&line, num);
        EncodeLineToObjectFile(ed->obj, line, ed->address);
        ClearLine(&line);
        ++ed->address;

        ed->fh->index = ParserSkipSeparator(ed->fh->line, ed->fh->index, ed->fh->bytes_read);
        ed->fh->index = ParserNextWord(ed->fh->line, ed->fh->word, ed->fh->index, ed->fh->bytes_read);
    }
}

int ExtractNibble(int line, int nibble_num)
{
    int nibble_mask = 0xF;
    int res = 0;

    res = (line >> (NIBBLE_SIZE * nibble_num)) & nibble_mask;

    return res;
}

void SetNum(int *line, int num)
{
    int num_mask = 0xFFFF;
    
    *line |= (num & num_mask);
}

void SetCodeInstruction(int *line, int type)
{
    *line |= (type << (NIBBLE_SIZE * (NUM_NIBBLES - 1)));
}

void ClearLine(int *line)
{
    int clear_mask = 0xF0000; /* doesn't clear instruction type */
    
    *line &= clear_mask;
}