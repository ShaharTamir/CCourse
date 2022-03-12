
#define _GNU_SOURCE
#include <stdio.h> /* FILE */

#include <node.h>
#include <linked_list.h>

#include "basic_defs.h"
#include "file_handler.h"
#include "parser.h"
#include "label.h"
#include "assembler.h"

#define ERR(string) printf("%serror: %s%s\n", CLR_RED, string, CLR_WHT)
#define ERR_AT(string, line) printf("%serror: %s at: %s%d%s\n", CLR_RED, string, CLR_YEL, line, CLR_WHT)
#define MEM_ADD_OFFSET 100

extern const char *g_instructions[NUM_INSTRUCTIONS];

typedef struct
{
    int data_count;
    int instruct_count;
    int status;
    SLinkedList *sym_table;
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
static int CheckNewLabel(SAssemblerData *data);
static void HandleNewLabelDef(SAssemblerData *data);
static void HandleNewInstructDef(SAssemblerData *data, int instruct);
static SLabel *AddLabelToSymTable(SAssemblerData *data);

void RunAssembler(FILE *in, char *file_name)
{
    SAssemblerData data;
    int status = 0;

    if(InitAssemblerData(&data))
    {
        status = DefineSymbolTable(in, &data);

        if(status)
        {
            printf("success!!!\n");
            /*
                OpenEntryFile();
                if(open)
                {
                    WriteEntryFile();
                }
                closeEntryFile();

                OpenExternFile();
                if(open)
                {
                    WriteExternFile();
                }

            */
        }
    }

    DestroyAssemblerData(&data);
}

int InitAssemblerData(SAssemblerData *data)
{
    int ret_val = FALSE;

    data->fh = CreateFileHandlerData(MAX_LINE_LENGTH, MAX_LABEL_NAME);
    
    if(data->fh)
    {
        data->sym_table = LinkListCreate(NULL, LabelCompareName);
        
        if(data->sym_table)
        {
            ret_val = TRUE;
            data->data_count = 0;
            data->instruct_count = 0;
        }
    }

    return ret_val;
}

void DestroyAssemblerData(SAssemblerData *data)
{
    if(data)
    {
        if(data->fh)
            DestroyFileHandlerData(data->fh);
        
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
    while(data->fh->bytes_read != EOF)
    {
        ++data->fh->line_count;
        data->fh->bytes_read = getline(&data->fh->line, &data->fh->line_len, in);
        
        data->fh->index = ParserNextWord(data->fh->line, data->fh->word, data->fh->index, data->fh->bytes_read);
        
        if(CheckNewLabel(data)) /* true means continue parsing line */
        {
            /*
            if(Instruction == data || string)
            {
                set label type as data;
                ++data_counter;
                verify syntax;
            }
            else // means it's a code line
            {
                if(is_label)
                    set label type as code;
                ++code_counter;
                according to opcode - verify syntax;
            }
            
            */
        }
    }

    return data->status;
}

int CheckNewLabel(SAssemblerData *data)
{
    int continue_read_line = TRUE;
    int instruct = FALSE;

    if(ParserIsNewLabel(data->fh->word))
    {
        HandleNewLabelDef(data);
    }
    else if(FALSE != (instruct = ParserIsExtEnt(data->fh->word)))
    {
        HandleNewInstructDef(data, instruct);
        continue_read_line = FALSE;
    }

    return continue_read_line;
}

void HandleNewLabelDef(SAssemblerData *data)
{
    SLabel *lbl = NULL;

    if(ParserValidateName(data->fh->word) && NULL == LinkListFind(data->sym_table, data->fh->word, NULL))
    {
        lbl = AddLabelToSymTable(data);
        /* TODO: LabelSetMemAddress(lbl,  + MEM_ADD_OFFSET); */
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
    SLabel *lbl = NULL;

    data->fh->index = ParserNextWord(data->fh->line, data->fh->word, data->fh->index, data->fh->bytes_read);

    if(ParserValidateName(data->fh->word))
    {
        if(NULL == (iter = LinkListFind(data->sym_table, data->fh->word, NULL)))
            lbl = AddLabelToSymTable(data);
        else
            lbl = (SLabel*)iter->data;
            
        if(LabelSetType(lbl, LabelInstructToLblType(instruct)))
        {
            if(ParserIsMoreWords(data->fh->line, data->fh->index, data->fh->bytes_read))
            {
                ERR_AT("invalid text after label definition", data->fh->line_count);
                data->status = FALSE;
            }   
        }
        else
        {
            /* if FALSE- LabelSetType already print the error, need line num */
            printf("%s at: %s%d%s\n", CLR_RED, CLR_YEL, data->fh->line_count, CLR_WHT);
            data->status = FALSE;
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