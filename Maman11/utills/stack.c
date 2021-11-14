#include <stdio.h>  /* printf */
#include <string.h> /* memcpy, memset */
#include <stdlib.h> /* malloc, free, realloc */

#include "stack.h"

#define PRINT_LOG 0
#define STACK_LOG(msg_string) if(PRINT_LOG){ printf("\n\nSTACK DEBUG: %s, %s, line: %d\n\n", msg_string, __FILE__, __LINE__); }

#define STACK_EMPTY -1
#define MIN_STACK_SIZE 10
#define MIN_DIFF_TO_RESIZE 5

struct _Stack
{
    int stack_size;
    int top_index;
    int var_size;
    void *stack;
};

static void StackResize(Stack* stack, int new_size);

Stack *StackCreate(int num_of_items, int item_size)
{
    Stack* new_stack = NULL;

    if(num_of_items <= 0 || item_size <= 0)
    {
        STACK_LOG("Num of items or item size are invalid!");
        return NULL;
    }

    new_stack = (Stack*) malloc(sizeof(Stack));
    
    if(NULL == new_stack)
    {
        STACK_LOG("Allocate Stack type malloc fail!");
        return NULL;
    }

    new_stack->stack = malloc(num_of_items * item_size);
            
    if(NULL == new_stack->stack)
    {
        STACK_LOG("Allocate stack mem malloc fail!");

        free(new_stack);
        new_stack = NULL;

        return NULL;
    }

    new_stack->stack_size = num_of_items;
    new_stack->top_index = STACK_EMPTY;
    new_stack->var_size = item_size;

    return new_stack;
}

void StackDestroy(Stack *stack)
{
    memset(stack->stack, 0, stack->stack_size * stack->var_size);
    free(stack->stack);
    memset(stack, 0, sizeof(Stack));
    free(stack);
    stack = NULL;
}

void StackPush(Stack *stack, void *input)
{
    if(NULL != stack && NULL != input)
    {
        if(stack->top_index + MIN_DIFF_TO_RESIZE >= stack->stack_size)
        {
            StackResize(stack, stack->stack_size * 2);
        }

        if((stack->top_index + 1) < stack->stack_size) /* stack index starts from 0. size starts from 1 */
        {
            ++stack->top_index;
            memcpy(&(((char*)stack->stack)[stack->top_index * stack->var_size]), input, stack->var_size);
        }
        else
        {
            STACK_LOG("stack is full!");
        }
    }
    else
    {
        STACK_LOG("stack or input is NULL!");
    }
}

void StackPeek(Stack *stack, void *output)
{
    if(NULL != stack && NULL != output)
    {
        if(stack->top_index >= 0) /* when stack is empty, top index is -1 */
        {
            memcpy(output, &((char*)stack->stack)[stack->top_index * stack->var_size], stack->var_size);
        }
        else
        {
            STACK_LOG("stack is empty!");
        } 
    }
    else
    {
        STACK_LOG("stack or output is NULL!");
    }
}

int StackIsEmpty(Stack *stack)
{
    return NULL == stack || stack->top_index == STACK_EMPTY;
}

void StackPop(Stack *stack, void *output)
{
    int resize_condition = 0;
    int half_size = 0;

    if(NULL == output)
    {
        STACK_LOG("output is null!");
        return;
    }

    if(!StackIsEmpty(stack))
    {
        memcpy(output, &((char*)stack->stack)[stack->top_index * stack->var_size], stack->var_size);
        --stack->top_index;
    }
    else
    {
        STACK_LOG("stack is empty!");
    }

    half_size = stack->stack_size / 2;
    resize_condition = half_size - MIN_DIFF_TO_RESIZE;

    if(resize_condition >= MIN_STACK_SIZE && stack->top_index + 1 <= resize_condition)
    {
        StackResize(stack, half_size);
    }
}

void StackEmptyStack(Stack *stack)
{
    if(stack)
    {
        stack->top_index = STACK_EMPTY;
    }
}

int StackGetIndex(Stack *s)
{
    if(s)
    {
        return s->top_index + 1;
    }

    return STACK_EMPTY;
}

void StackResize(Stack* stack, int new_size)
{
    void *resized_stack = NULL;

    if(new_size > stack->stack_size)
        resized_stack = realloc(stack->stack, stack->var_size * new_size);
    else
        resized_stack = malloc(stack->var_size * new_size);
    
    if(resized_stack != NULL)
    {
        if(new_size < stack->stack_size)
        {
            memcpy(resized_stack, stack->stack, stack->var_size * new_size);
            free(stack->stack);
            stack->stack = NULL;
        }

        stack->stack = resized_stack;
        stack->stack_size = new_size;

        if(stack->top_index >= stack->stack_size)
        {
            STACK_LOG("index is out of bound! ABORT!");
            exit(-1);
        }
    }
    else
    {
        STACK_LOG("no more memory to realloc! stack is complitly full!!");
    }
}