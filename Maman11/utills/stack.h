#ifndef __STACK_H_CCOURSE_
#define __STACK_H_CCOURSE_

typedef struct _Stack Stack;

Stack *StackCreate(int num_of_items, int item_size);

void StackDestroy(Stack *stack);

void StackPush(Stack *stack, void *input);

void StackPeek(Stack *stack, void *output);

int StackIsEmpty(Stack *stack);

void StackPop(Stack *stack, void *output);

void StackEmptyStack(Stack *stack);

int StackGetIndex(Stack *stack);

#endif /* __STACK_H_CCOURSE_ */