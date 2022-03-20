#include <stdio.h> /* fprintf */
#include <stdlib.h> /* free */
#include <string.h> /* memset */

#include "basic_defs.h"
#include "label.h"
#include "file_handler.h"
#include "linked_list.h"
#include "parser.h"
#include "function.h"
#include "encoder.h"

#define START_ADD 100
#define FOUR_DIG 1000
#define DEC_BASE 10
#define NUM_PARAMS 2
#define NUM_NIBBLES 5
#define NIBBLE_SIZE 4
#define INST_START_BIT 16
#define FUNCT_START_BIT 12
#define SRC_REG_START_BIT 8
#define SRC_ADDRESS_MODE_START_BIT 6
#define DEST_REG_START_BIT 2
#define DEST_ADDRESS_MODE_START_BIT 0
#define STRING_CLOSERS 2
#define MAX_NUM 32767
#define MIN_NUM -32768

extern const SFunctions g_func_names[NUM_FUNCTIONS];

typedef enum
{
    CODE_ABSOLUTE = 4,      /* 0100 */
    CODE_RELOCATABLE = 2,   /* 0010 */
    CODE_EXTERNAL = 1       /* 0001 */
} EInstType;

static int ExtractNibble(int line, int nibble_num);
static void SetToLine(int *line, int var, int offset);
static void SetNum(int *line, int num);
static void ClearLine(int *line);

void EncodeString(SEncoderData *ed);
void EncodeData(SEncoderData *ed);
int EncodeFunction(SEncoderData *ed);
void EncodeFunctLine(SEncoderData *ed, int *line, int func_index);
int EncodeVariables(SEncoderData *ed, int *line, int func_index);
void EncodeLbl(SEncoderData *ed, SLabel *lbl);

/* ToFiles encoder functions */
void EncodeLblToEntryFile(FILE *out, SLabel *lbl);
void EncodeLblToExtFile(FILE *out, char *lbl_name, int address);
void EncodeLineToObjectFile(FILE *out, int line, int *address);

void EncodeLblToExtFile(FILE *out, char *lbl_name, int address)
{
    char base[] = {" BASE "};
    char offset[] = {" OFFSET "};

    fprintf(out, "%s%s%d\n", lbl_name, base, address);
    fprintf(out, "%s%s%d\n\n", lbl_name, offset, address + 1);
}

void EncodeLblToEntryFile(FILE *out, SLabel *lbl)
{
    if(LabelIsAlsoEntry(lbl))
        fprintf(out, "%s,%d,%d\n", LabelGetName(lbl), LabelGetBaseAddress(lbl), LabelGetOffset(lbl));
}

void EncodeLineToObjectFile(FILE *out, int line, int *address)
{
    char nibble_part[] = {'E','D','C','B','A'};
    int i = 0;

    if(*address < FOUR_DIG)
        fprintf(out, "0%d ", *address);
    else
        fprintf(out, "%d ", *address);

    for(i = NIBBLE_SIZE; i > 0; --i)
    {
        fprintf(out, "%c%x-", nibble_part[i], ExtractNibble(line, i));
    }

    fprintf(out, "%c%x\n", nibble_part[i], ExtractNibble(line, i));
    ++*address;
}

void EncodeLbl(SEncoderData *ed, SLabel *lbl)
{
    int line = 0;

    if(LabelIsExtern(lbl))
    {
        EncodeLblToExtFile(ed->ext, LabelGetName(lbl), ed->address);

        SetToLine(&line, CODE_EXTERNAL, INST_START_BIT);
        EncodeLineToObjectFile(ed->obj, line, &ed->address);
        EncodeLineToObjectFile(ed->obj, line, &ed->address);
    }
    else
    {
        /* To Object file */
        SetToLine(&line, CODE_RELOCATABLE, INST_START_BIT);
        line += LabelGetBaseAddress(lbl);
        EncodeLineToObjectFile(ed->obj, line, &ed->address); /* base */
        line = line - LabelGetBaseAddress(lbl) + LabelGetOffset(lbl);
        EncodeLineToObjectFile(ed->obj, line, &ed->address); /* offset */
    }
}

int InitEncoderData(SFileHandlerData *fh, SLinkedList *sym_table, SEncoderData *en_data, \
                const char *file_name, int open_ent, int open_ext)
{
    char *new_file_name = NULL;

    en_data->obj = NULL;
    en_data->ent = NULL;
    en_data->ext = NULL;

    new_file_name = FileHandlerGetFileName(file_name, STAGE_OBJ);
    en_data->obj = FileHandlerOpenFile(new_file_name, "w");
    free(new_file_name);

    if(!en_data->obj)
        return FALSE;

    if(open_ext)
    {
        new_file_name = FileHandlerGetFileName(file_name, STAGE_EXT);
        en_data->ext = FileHandlerOpenFile(new_file_name, "w");
        free(new_file_name);
        
        if(!en_data->ext)
            return FALSE;
    }

    if(open_ent)
    {
        new_file_name = FileHandlerGetFileName(file_name, STAGE_ENT);
        en_data->ent = FileHandlerOpenFile(new_file_name, "w");
        free(new_file_name);

        if(!en_data->ent)
            return FALSE;
    }

    en_data->is_data_encode = FALSE;
    en_data->fh = fh;
    en_data->sym_tbl = sym_table;
    en_data->address = START_ADD;
    en_data->fh->line_count = 0;

    return TRUE;
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

void EncodeObjHeadline(SEncoderData *en_data, int DC, int IC)
{
    fprintf(en_data->obj, "\t%d\t%d\n", IC, DC);
}

int EncodeLine(SEncoderData *ed)
{
    int instruction = 0;

    ed->fh->index = ParserNextWord(ed->fh->line, ed->fh->word, ed->fh->index, ed->fh->bytes_read);

    if(!ParserIsExtEnt(ed->fh->word)) /* skip .entry and .extern lines */
    {
        if(ParserIsNewLabel(ed->fh->word)) /* no encode in obj file for label def */
        {
            if(ed->is_data_encode)
                EncodeLblToEntryFile(ed->ent, LinkListFind(ed->sym_tbl, ed->fh->word, NULL)->data);

            ed->fh->index = ParserNextWord(ed->fh->line, ed->fh->word, ed->fh->index, ed->fh->bytes_read);
        }

        instruction = ParserIsDataString(ed->fh->word);

        if(instruction && ed->is_data_encode)
        {
            if(INST_STRING == instruction)
                EncodeString(ed);
            else
                EncodeData(ed);
        }
        else if(!instruction && !ed->is_data_encode) /* code */
        {
            return EncodeFunction(ed);
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
    SetToLine(&line, CODE_ABSOLUTE, INST_START_BIT);

    for(i = 1; i <= len; ++i)
    {
        line += (int)(ed->fh->word[i]); /* char is also int */
        EncodeLineToObjectFile(ed->obj, line, &ed->address);
        line -= (int)(ed->fh->word[i]); /* reset line for next char */
    }

    EncodeLineToObjectFile(ed->obj, line, &ed->address); /* null terminator */
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
    SetToLine(&line, CODE_ABSOLUTE, INST_START_BIT);

    for(i = 0; i < num_vars; ++i)
    {
        num = (int)strtol(ed->fh->word, &next_data, DEC_BASE);
        if(num > MAX_NUM || num < MIN_NUM)
            WARNING_AT("number out of size range, behaviour undefined", ed->fh->line_count);

        SetNum(&line, num);
        EncodeLineToObjectFile(ed->obj, line, &ed->address);
        ClearLine(&line);

        ed->fh->index = ParserSkipSeparator(ed->fh->line, ed->fh->index, ed->fh->bytes_read);
        ed->fh->index = ParserNextWord(ed->fh->line, ed->fh->word, ed->fh->index, ed->fh->bytes_read);
    }
}

int EncodeFunction(SEncoderData *ed)
{
    int func_index = 0;
    int line = 0;
    int params_start_index = 0;

    SetToLine(&line, CODE_ABSOLUTE, INST_START_BIT);
    
    /* handle opcode line */
    func_index = ParserIsFunction(ed->fh->word) - 1;
    SetToLine(&line, 1, g_func_names[func_index].opcode);
    EncodeLineToObjectFile(ed->obj, line, &ed->address);
    params_start_index = ed->fh->index; /* save start of parameters before logic */
    
    /* handle line 2 funct */
    EncodeFunctLine(ed, &line, func_index);

    /* handle each param encoding (mem addresses and immidiate vars)*/
    ed->fh->index = params_start_index;
    return EncodeVariables(ed, &line, func_index);
}

void EncodeFunctLine(SEncoderData *ed, int *line, int func_index)
{
    int reg_bit[] = {DEST_REG_START_BIT, SRC_REG_START_BIT};
    int acc_bit[] = {DEST_ADDRESS_MODE_START_BIT, SRC_ADDRESS_MODE_START_BIT};
    int access_meth = 0;
    int i = 0;

    ClearLine(line);
    SetToLine(line, g_func_names[func_index].funct, FUNCT_START_BIT);

    for(i = g_func_names[func_index].num_params - 1; i >= 0; --i)
    {
        ed->fh->index = ParserSkipSeparator(ed->fh->line, ed->fh->index, ed->fh->bytes_read);
        ed->fh->index = ParserNextWord(ed->fh->line, ed->fh->word, ed->fh->index, ed->fh->bytes_read);
        ParserCleanSeparator(ed->fh->word);
        access_meth = FunctionGetAccessingMethod(ed->fh->word);
        SetToLine(line, access_meth - 1, acc_bit[i]); /* set funct line accessing methods */

        /* handle registers in funct line */
        if(access_meth == AC_REG)
        {
            SetToLine(line, ParserIsRegister(ed->fh->word), reg_bit[i]);
        }
        else if(access_meth == AC_INDEX)
        {
            /* extract register from index */
            SetToLine(line, ParserIndexNum(ed->fh->word), reg_bit[i]);
        }

    }
    
    if(g_func_names[func_index].num_params) /* do not write funct line for fuctions without parameters */
        EncodeLineToObjectFile(ed->obj, *line, &ed->address);
}

int EncodeVariables(SEncoderData *ed, int *line, int func_index)
{
    char index_dummy_word[INDEX_LENGTH + 1] = {0};
    SNode *label_node = NULL;
    int access_meth = 0;
    int i = 0;
    int ret_val = TRUE;

    for(i = 0; i < g_func_names[func_index].num_params; ++i)
    {
        ClearLine(line);
        ed->fh->index = ParserSkipSeparator(ed->fh->line, ed->fh->index, ed->fh->bytes_read);
        ed->fh->index = ParserNextWord(ed->fh->line, ed->fh->word, ed->fh->index, ed->fh->bytes_read);
        ParserCleanSeparator(ed->fh->word);
        access_meth = FunctionGetAccessingMethod(ed->fh->word);

        switch(access_meth)
        {
            case AC_IMMEDIATE:
                SetNum(line, (int)strtol(ed->fh->word + 1, NULL, DEC_BASE));
                EncodeLineToObjectFile(ed->obj, *line, &ed->address);
                break;
            case AC_INDEX:
                ParserExtractIndexFromWord(ed->fh->word, index_dummy_word);
            case AC_DIRECT:
                ret_val = (NULL != (label_node = LinkListFind(ed->sym_tbl, ed->fh->word, NULL)));
                if(label_node)
                {
                    EncodeLbl(ed, label_node->data);
                }
                break;
        }
    }

    if(!ret_val)
        ERR_AT("label is used but not defined!", ed->fh->line_count);

    return ret_val;
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

void SetToLine(int *line, int var, int offset)
{
    *line |= (var << offset);
}

void ClearLine(int *line)
{
    int clear_mask = 0xF0000; /* doesn't clear instruction type */
    
    *line &= clear_mask;
}