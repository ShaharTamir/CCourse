#ifndef __STACK_H_CCOURSE_
#define __STACK_H_CCOURSE_

typedef struct
{
    void *stack;
    int stack_size;
    int top_index;
    int var_size;
} Stack;

Stack *StackCreate(int num_of_items, int item_size);

void StackDestroy(Stack *stack);

void StackPush(Stack *stack, void *input);

void StackPeek(Stack *stack, void *output);

int StackIsEmpty(Stack *stack);

void StackPop(Stack *stack, void *output);

void StackEmptyStack(Stack *stack);

int StackGetIndex(Stack *stack);

/*int StackSize(Stack* stack);*/

/*Stack *Resize(Stack *stack, int new_size);*/

#endif /* __STACK_H_CCOURSE_ */