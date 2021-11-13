#include <stdio.h>  /* printf */
#include <stdlib.h> /* free */

#include "tester.h"
#include "stack.h"

struct _Stack
{
    int stack_size;
    int top_index;
    int var_size;
    void *stack;
};

void TestStackCreate(TestStatusType*);
void TestPush(TestStatusType*);
void TestPeek(TestStatusType*);
void TestIsEmpty(TestStatusType*);
void TestPop(TestStatusType*);

TestFunction tests[] = {TestStackCreate, TestPush, TestPeek, TestIsEmpty, TestPop};

int main()
{
    TestStatusType status = {0, 0, 0};
    
    RunTests(tests, &status, sizeof(tests) / sizeof(TestFunction));
    PrintSummary(&status);

    return 0;
}

void TestStackCreate(TestStatusType* status)
{
    Stack* s = NULL;
    int valid_var_size = 1;
    int valid_num_items = 3;
    int exp_stack_size = 3;

    PrepareForTest("Create invalid items number", status);
    s = StackCreate(0, 1);
    CheckResult((s == NULL), __LINE__, status);

    PrepareForTest("Create invalid item size", status);
    s = StackCreate(2, 0);
    CheckResult((s == NULL), __LINE__, status);

    PrepareForTest("Create a valid stack memory allocation", status);
    s = StackCreate(valid_num_items, valid_var_size);
    CheckResult((s != NULL && s->stack != NULL), __LINE__, status);

    PrepareForTest("Verify init top index", status);
    CheckResult(StackGetIndex(s) == 0, __LINE__, status);

    PrepareForTest("Verify init var size", status);
    CheckResult(s->var_size == valid_var_size, __LINE__, status);

    PrepareForTest("Verify init stack size", status);
    CheckResult(s->stack_size == exp_stack_size, __LINE__, status);

    StackDestroy(s);
}

void TestPush(TestStatusType* status)
{
    Stack* s = NULL;
    int num_items = 50;
    int num_small_items = 2;
    int item_size = 1;
    char item = 0;

    s = StackCreate(num_items, item_size);
    PrepareForTest("Test a push by top index update", status);
    item = '{';
    StackPush(s, (void*)&item);
    CheckResult(StackGetIndex(s) == 1, __LINE__, status);

    PrepareForTest("Next Push", status);
    StackPush(s, (void*)&item);
    CheckResult(StackGetIndex(s) == 2, __LINE__, status);

    StackDestroy(s);

    s = StackCreate(num_small_items, item_size);
    PrepareForTest("Test resizing the stack", status);
    StackPush(s, (void*)&item);
    CheckResult(s->stack_size == num_small_items * 2, __LINE__, status);

    StackDestroy(s);
}

void TestPeek(TestStatusType* status)
{
    Stack* s = NULL;
    int num_items = 50;
    int item_size = 1;
    char item = 0;
    char special_top = '}';
    char output = 0;
    int i = 0;

    PrepareForTest("One push and verify top", status);
    s = StackCreate(num_items, item_size);
    item = '{';
    StackPush(s, &item);
    StackPeek(s, &output);
    CheckResult(item == output, __LINE__, status);

    PrepareForTest("Filling up the stack - last output", status);
    for(i = 0; i < num_items; ++i)
    {
        if(i == num_items - 1)
            item = special_top;
        else
            item = '{';
        StackPush(s, &item);    
    }

    StackPeek(s, &output);
    CheckResult(special_top == output, __LINE__, status);
    PrepareForTest("Top index is not stack size because size increased", status);
    CheckResult(StackGetIndex(s) + 1 != s->stack_size, __LINE__, status);

    StackDestroy(s);
}

void TestIsEmpty(TestStatusType* status)
{
    Stack* s = NULL;
    int num_items = 50;
    int item_size = 1;
    char item = '}';

    PrepareForTest("Is empty when stack is NULL", status);
    CheckResult(StackIsEmpty(s), __LINE__, status);
    
    PrepareForTest("Is empty after creation", status);
    s = StackCreate(num_items, item_size);
    CheckResult(StackIsEmpty(s), __LINE__, status);

    PrepareForTest("Peek when stack is empty", status);
    StackPeek(s, &item);
    CheckResult(item == '}', __LINE__, status);

    PrepareForTest("Not empty after push", status);
    StackPush(s, &item);
    CheckResult(!StackIsEmpty(s), __LINE__, status);

    StackDestroy(s);
}

void TestPop(TestStatusType* status)
{
    Stack* s = NULL;
    int num_items = 50;
    int item_size = 1;
    char item = '}';
    char output = 0;

    PrepareForTest("Push an item, make sure not empty, pop and verify empty", status);
    s = StackCreate(num_items, item_size);
    StackPush(s, &item);
    if(!StackIsEmpty(s) && StackGetIndex(s) > 0)
    {
        StackPop(s, &output);
        CheckResult(output == item && StackIsEmpty(s) 
            && StackGetIndex(s) == 0, __LINE__, status);
    }
    else
    {
        CheckResult(TEST_FAIL, __LINE__, status);
    }

    StackDestroy(s);
}