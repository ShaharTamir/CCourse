
#define _GNU_SOURCE
#include <stdio.h> /* FILE, fseek */

#include <node.h>
#include <linked_list.h>

#include "basic_defs.h"
#include "file_handler.h"
#include "parser.h"
#include "label.h"
#include "function.h"
#include "encoder.h"
#include "assembler.h"

#define MEM_ADD_OFFSET 100

typedef struct
{
    int data_count;
    int instruct_count;
    int status;
    SLinkedList *sym_table;
    SLabel *lbl;
    SFileHandlerData *fh;
} SAssemblerData;

static int InitAssemblerData(SAssemblerData *data);
static void DestroyAssemblerData(SAssemblerData *data);

/* 
    This function is the main logic and has 3 jobs - 
    1. Define all labels in code and add their type: EXTERN, ENTRY, DATA or CODE.
    2. verify the written code in file is valid. if not - print errors.
    3. count how many code lines and how many data lines are there.

    if this function return TRUE:
        means file syntax is valid and all labels are defined.
        Then we proceed to encoding the output files.
    else
        We finish the program without the output files. Errors will be printed to 
        the standard output.
 */
static int DefineSymbolTable(FILE *in, SAssemblerData *data);

/* FIRST CYCLE FUNCTIONS */
static int CheckNewLabel(SAssemblerData *data);
static void HandleNewLabelDef(SAssemblerData *data);
static void HandleNewInstructDef(SAssemblerData *data, int instruct);
static SLabel *AddLabelToSymTable(SAssemblerData *data);
static int AddLabelTypeCodeData(SAssemblerData *data, int instruction);
static void HandleString(SAssemblerData *data);
static void HandleData(SAssemblerData *data);
static void HandleCode(SAssemblerData *data);

void RunAssembler(FILE *in, char *file_name)
{
    SAssemblerData data;
    SEncoderData en_data;
    int status = 0;

    if(InitAssemblerData(&data))
    {
        status = DefineSymbolTable(in, &data);

        if(status)
        {
            fseek(in, 0, SEEK_SET); /* reset file to begin */
            if(InitEncoderData(data.fh, data.sym_table, &en_data, file_name))
            {
                en_data.fh->bytes_read = getline(&en_data.fh->line, &en_data.fh->line_len, in);

                while(en_data.fh->bytes_read != EOF)
                {
                    ++en_data.fh->line_count;
                    en_data.fh->index = 0;
                    if(!EncodeLine(&en_data))
                    {
                        ERR_AT("fuck everythings over", en_data.fh->line_count);
                        /*fcloseall();
                        FileHandlerRemoveAll(file_name);*/
                        break;
                    }
                    en_data.fh->bytes_read = getline(&en_data.fh->line, &en_data.fh->line_len, in);
                }
            }

            DestroyEncoderData(&en_data);
            /*printf("num code blocks: %d \
                  \nnum data blocks: %d - success!!!\n", data.instruct_count, data.data_count);
            printf("expected: \
                \n3 = 4\n4 = 7\n5 = 11\n6 = 13\n7 = 17\n8 = 19 \
                \n9 = 23\n10 = 28\n11 = 32\n12 = 36\n13 = 40\n14 = 41\n");*/
        }
    }

    DestroyAssemblerData(&data);
}

int InitAssemblerData(SAssemblerData *data)
{
    int ret_val = FALSE;

    data->fh = FileHandlerCreate(MAX_LINE_LENGTH, MAX_LINE_LENGTH);
    
    if(data->fh)
    {
        data->sym_table = LinkListCreate(NULL, LabelCompareName);
        
        if(data->sym_table)
        {
            ret_val = TRUE;
            data->status = TRUE;
            data->data_count = 0;
            data->instruct_count = 0;
            data->lbl = NULL;
        }
    }

    return ret_val;
}

void DestroyAssemblerData(SAssemblerData *data)
{
    if(data)
    {
        if(data->fh)
            FileHandlerDestroy(data->fh);
        
        if(data->sym_table)
        {
            LinkListForEach(data->sym_table, LabelDestroyWrapper, NULL); /* release all allocations for macro names */
            LinkListDestroy(data->sym_table);
            data->sym_table = NULL;
        }
    }
}

int DefineSymbolTable(FILE *in, SAssemblerData *data)
{
    int instruction = FALSE;

    while(data->fh->bytes_read != EOF)
    {
        ++data->fh->line_count;
        data->fh->index = 0;
        data->lbl = NULL;
        data->fh->bytes_read = getline(&data->fh->line, &data->fh->line_len, in);
        data->fh->index = ParserNextWord(data->fh->line, data->fh->word, data->fh->index, data->fh->bytes_read);
        
        if(CheckNewLabel(data)) /* true means continue parsing line */
        {
            if(data->lbl)
                data->fh->index = ParserNextWord(data->fh->line, data->fh->word, data->fh->index, data->fh->bytes_read);
            
            instruction = ParserIsDataString(data->fh->word);

            if(data->lbl)
                AddLabelTypeCodeData(data, instruction);

            if(instruction)
            {
                if(INST_STRING == instruction)
                    HandleString(data);
                else
                    HandleData(data);
            }
            else
            {
                HandleCode(data);
            }
        }
    }

    return data->status;
}

int CheckNewLabel(SAssemblerData *data)
{
    int continue_read_line = FALSE;
    int instruct = FALSE;

    if(ParserIsNewLabel(data->fh->word))
    {
        continue_read_line = TRUE;
        HandleNewLabelDef(data);
    }
    else if(FALSE != (instruct = ParserIsExtEnt(data->fh->word)))
    {
        HandleNewInstructDef(data, instruct);
    }
    else if(data->fh->bytes_read != EOF)
    {
        continue_read_line = TRUE;
    }

    return continue_read_line;
}

void HandleNewLabelDef(SAssemblerData *data)
{
    SNode *iter = NULL;

    if(ParserValidateName(data->fh->word) && NULL == (iter = LinkListFind(data->sym_table, data->fh->word, NULL)))
    {
        data->lbl = AddLabelToSymTable(data);
        LabelSetMemAddress(data->lbl, data->data_count + data->instruct_count + MEM_ADD_OFFSET);
    }
    else if(iter && LabelIsEntry(iter->data)) /* label is defined only as entry but not yet defined */
    {
        data->lbl = iter->data;
        LabelSetMemAddress(data->lbl, data->data_count + data->instruct_count + MEM_ADD_OFFSET);
    }
    else
    {
        ERR_AT("label name is invalid", data->fh->line_count);
        data->status = FALSE;
    }
}

void HandleNewInstructDef(SAssemblerData *data, int instruct)
{
    SNode *iter = NULL;

    data->fh->index = ParserNextWord(data->fh->line, data->fh->word, data->fh->index, data->fh->bytes_read);
    if(ParserValidateName(data->fh->word))
    {
        if(NULL == (iter = LinkListFind(data->sym_table, data->fh->word, NULL)))
            data->lbl = AddLabelToSymTable(data);
        else
            data->lbl = (SLabel*)iter->data;
            
        if(AddLabelTypeCodeData(data, instruct))
        {
            if(ParserIsMoreWords(data->fh->line, data->fh->index, data->fh->bytes_read))
            {
                ERR_AT("invalid text after label definition", data->fh->line_count);
                data->status = FALSE;
            }   
        }
    }
    else
    {
        ERR_AT("label name is invalid", data->fh->line_count);
        data->status = FALSE;
    }
}

SLabel *AddLabelToSymTable(SAssemblerData *data)
{
    SNode *iter = NULL;

    iter = NodeCreate(LabelCreate(data->fh->word), NULL);
    LinkListAppend(data->sym_table, iter);
    
    return iter->data;
}

int AddLabelTypeCodeData(SAssemblerData *data, int instruction)
{
    int inst_type = FALSE;

    inst_type = instruction ? LabelInstructToLblType(instruction) : LBL_CODE;

    if(!LabelSetType(data->lbl, inst_type))
    {
        /* if FALSE- LabelSetType already print the error, need line num */
        printf("%s at: %s%d%s\n", CLR_RED, CLR_YEL, data->fh->line_count, CLR_WHT);
        data->status = FALSE;
        return FALSE;
    }

    return TRUE;
}

void HandleString(SAssemblerData *data)
{
    int string_len = 0;
    
    string_len = ParserIsValidString(data->fh->line, data->fh->index, data->fh->bytes_read);
    if(string_len) /* not 0 means valid */
    {
         data->data_count += string_len;
    }
    else
    {
        ERR_AT("string is not valid", data->fh->line_count);
        data->status = FALSE;
    }
}

void HandleData(SAssemblerData *data)
{
    if(ParserIsValidData(data->fh->line, data->fh->index, data->fh->bytes_read))
    {
        data->data_count += ParserCountSeparators(data->fh->line, data->fh->index, data->fh->bytes_read) + 1;
    }
    else
    {
        ERR_AT("data is not valid", data->fh->line_count);
        data->status = FALSE;
    }
}

void HandleCode(SAssemblerData *data)
{
    int func = FALSE;

    func = ParserIsFunction(data->fh->word);

    if(func)
    {
        if(!FunctionValidateFunc(data->fh, &data->instruct_count, func))
        {
            ERR_AT("invalid function parameters", data->fh->line_count);
            data->status = FALSE;
        }
        /* DELETE printf("line: %d, instruct_count: %d\n", data->fh->line_count, data->instruct_count); */
    }
    else
    {
        ERR_AT("invalid line - function is missing", data->fh->line_count);
        data->status = FALSE;
    }
}



