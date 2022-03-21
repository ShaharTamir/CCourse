
#define _GNU_SOURCE
#include <stdio.h> /* FILE, fseek */

#include "node.h"
#include "linked_list.h"

#include "basic_defs.h"
#include "file_handler.h"
#include "parser.h"
#include "label.h"
#include "function.h"
#include "encoder.h"
#include "assembler.h"

typedef struct
{
    int data_count;
    int instruct_count;
    int status;
    int open_ent;
    int open_ext;
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

/* logic of writing to files using encoder.c API */
static void EncodeOutput(FILE *in, SAssemblerData *data, char *file_name);

/* FIRST CYCLE FUNCTIONS */

/* handle adding labels to sym_table and if should continue parse line */
static int CheckNewLabel(SAssemblerData *data);

/* handle new label defined with ':' */
static void HandleNewLabelDef(SAssemblerData *data);

/* handle new label defined with .entry or .extern */
static void HandleNewInstructDef(SAssemblerData *data, int instruct);

/* handle and validate .string instruction */
static void HandleString(SAssemblerData *data);

/* handle and validate .data instruction */
static void HandleData(SAssemblerData *data);
static void HandleCode(SAssemblerData *data);

/* SETTING LABEL SERVICE FUNCTIONS */
static SLabel *AddLabelToSymTable(SAssemblerData *data);
static int AddLabelTypeCodeData(SAssemblerData *data, int instruct);

void RunAssembler(FILE *in, char *file_name)
{
    SAssemblerData data;
    
    int status = 0;

    if(InitAssemblerData(&data))
    {
        status = DefineSymbolTable(in, &data);
        EncodeOutput(in, &data, file_name);

        if(!status || !data.status)
        {
            printf("%sin file: %s.am%s\n\n", CLR_BOLD, file_name, CLR_RESET);
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
            data->open_ext = FALSE;
            data->open_ent = FALSE;
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
        
        if(!ParserValidateLineLen(data->fh->bytes_read))
        {
            ERR_AT("inavlid line length - behaviour is undefined", data->fh->line_count);
            data->status = FALSE;
        }

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

    /* calc data labels (only) relative mem address according to data count at each label def and total instructions count */
    LinkListForEach(data->sym_table, LabelCalcDataMemAddress, &data->instruct_count);

    return data->status;
}

void EncodeOutput(FILE *in, SAssemblerData *data, char *file_name)
{
    SEncoderData en_data;

    if(InitEncoderData(data->fh, data->sym_table, &en_data, file_name, 
        data->open_ent, data->open_ext))
    {
        EncodeObjHeadline(&en_data, data->data_count, data->instruct_count);
        
        /* 2 cycles - first is for code instructions, 2nd is for data */
        for(en_data.is_data_encode = FALSE; en_data.is_data_encode <= TRUE; ++en_data.is_data_encode)
        {
            en_data.fh->line_count = 0;
            fseek(in, 0, SEEK_SET); /* reset file to begin */
            en_data.fh->bytes_read = getline(&en_data.fh->line, &en_data.fh->line_len, in);

            while(en_data.fh->bytes_read != EOF)
            {
                ++en_data.fh->line_count;
                en_data.fh->index = 0;
                if(!EncodeLine(&en_data)) /* FALSE means something went wrong */
                {
                    data->status = FALSE;
                    break;
                }
                en_data.fh->bytes_read = getline(&en_data.fh->line, &en_data.fh->line_len, in);
            }
        }
    }

    DestroyEncoderData(&en_data);
    if(!data->status)
        FileHandlerRemoveAll(file_name);
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
    else if(FALSE != (instruct = ParserIsExtEnt(data->fh->word))) /* word is .entry or .extern */
    {
        /* set to open relevant file when encoding */
        (instruct == INST_ENTRY) ? (data->open_ent = TRUE) : (data->open_ext = TRUE); 
        HandleNewInstructDef(data, instruct); /* validate and add name to sym_table*/
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

    /* validate name and verify not exist in sym_table */
    if(ParserValidateName(data->fh->word))
    {
        if(NULL == (iter = LinkListFind(data->sym_table, data->fh->word, NULL)))
        {
            data->lbl = AddLabelToSymTable(data);
        }
        else if(iter && LabelIsEntry(iter->data)) /* label is defined only as entry but memory not yet defined */
        {
            data->lbl = iter->data;
        }
        else
        {
            data->lbl = iter->data;
            ERR_AT("label is defined twice", data->fh->line_count);
            data->status = FALSE;
        }
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
        if(NULL == (iter = LinkListFind(data->sym_table, data->fh->word, NULL))) /* check if exist in sym_table */
            data->lbl = AddLabelToSymTable(data);
        else
            data->lbl = (SLabel*)iter->data; /* means exist */
            
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

void HandleString(SAssemblerData *data)
{
    int string_len = 0;
    
    string_len = ParserIsValidString(data->fh->line, data->fh->index, data->fh->bytes_read);
    if(string_len) /* not 0 means valid */
    {
        LabelSetNumEncodeblks(data->lbl, data->data_count); /* if lbl == NULL does nothing */
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
        LabelSetNumEncodeblks(data->lbl, data->data_count); /* if lbl == NULL does nothing */
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

    if(data->lbl)
        LabelSetMemAddress(data->lbl, data->instruct_count); /* set code label definitions first */

    func = ParserIsFunction(data->fh->word); /* get func index or FALSE */

    if(func)
    {
        if(!FunctionValidateFunc(data->fh, &data->instruct_count, func))
        {
            ERR_AT("invalid function parameters", data->fh->line_count);
            data->status = FALSE;
        }
    }
    else
    {
        ERR_AT("invalid line - function is missing", data->fh->line_count);
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
        printf("\tat: %s%d%s\n", CLR_YEL, data->fh->line_count, CLR_RESET);
        data->status = FALSE;
        return FALSE;
    }

    return TRUE;
}
