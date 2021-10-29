#include <stdio.h>  /* printf */
#include <string.h> /* memcpy, memset */
#include <stdlib.h> /* malloc, free */

#include "stack.h"

#define STACK_EMPTY -1
#define STACK_LOG(msg_string) (printf("\n\nSTACK DEBUG: %s, %s, line: %d\n\n", msg_string, __FILE__, __LINE__))

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
    new_stack->top_index = -1;
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
        if((stack->top_index + 1) < stack->stack_size)
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
}

int StackGetIndex(Stack *s)
{
    if(s)
    {
        return s->top_index + 1;
    }

    return -1;
}