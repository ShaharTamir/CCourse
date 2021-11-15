#ifndef __STACK_H_CCOURSE_
#define __STACK_H_CCOURSE_

/*************************************************************
 * Stack.h
 * 
 * This API is for a generic dynamic stack.
 * 
 * Generic means it can contain any data type, as long as given
 * the size of a single item.
 * 
 * Dynamic means it's managing it's own memory and can resize
 * according to the used capacity of the stack.
**************************************************************/

/* Encapsulated Stack struct */
typedef struct _Stack Stack;

/**
 * StackCreate
 * 
 * starting_size: starting capacity of items for the stack. must be a possitive number.
 * item_size: size in bytes. must be a possitive number.
 * 
 * return: Success - a pointer to a new empty stack.
 *         Failure - NULL.
*/
Stack *StackCreate(int starting_size, int item_size);

/**
 * StackDestroy
 * 
 * stack: ptr to stack.
 * 
 * return: NONE
*/
void StackDestroy(Stack *stack);

/** 
 * StackPush - push a new item into the stack
 * 
 * stack: ptr to stack.
 * input: ptr to input to push to stack. 
 *   if input size > item_size - the stack will copy only item_size bytes.
 *   if input size < item_size - the behaviour is undefined.
 * 
 * return: NONE  
*/ 
void StackPush(Stack *stack, void *input);

/**
 * StackPeek - get a copy of the top item in stack
 * 
 * stack: ptr to stack.
 * output: ptr to memory to copy stack top into.
 *      if the stack is empty - output is untouched.
 * 
 * return: NONE 
*/ 
void StackPeek(Stack *stack, void *output);

/**
 * StackIsEmpty
 * 
 * stack: ptr to stack.
 * 
 * return: Empty == 1, else == 0 
*/
int StackIsEmpty(Stack *stack);

/**
 * StackPop - remove the top item from the stack
 * 
 * stack: ptr to stack.
 * output: ptr to memory to copy poped value into.
 *      if the stack is empty - output is untouched.
 * 
 * return: NONE 
*/ 
void StackPop(Stack *stack, void *output);

/**
 * StackEmptyStack - Empties the stack
 * 
 * stack: ptr to stack.
 * 
 * return: NONE 
*/
void StackEmptyStack(Stack *stack);

/**
 * StackGetIndex
 * 
 * stack: ptr to stack.
 * 
 * return: The current number of items in stack. 
*/
int StackGetIndex(Stack *stack);

#endif /* __STACK_H_CCOURSE_ */