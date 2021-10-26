#include <stdio.h>  /* printf */
#include <stdlib.h> /* free */
#include "stack.h"

typedef enum
{
    FAIL,
    PASS
} EStatus;

typedef struct
{
    int test_num;
    int num_pass;
    int num_fail;
} StatusType;

void TestStackCreate(StatusType*);
void TestPush(StatusType*);
void TestPeek(StatusType*);
void TestIsEmpty(StatusType*);
void TestPop(StatusType*);

void PrepareForTest(char *desc, StatusType* status);
void CheckResult(int condition, int line_of_failure, StatusType* status);
void PrintSummary(StatusType* status);

int main()
{
    StatusType status = {0, 0, 0};
    TestStackCreate(&status);
    TestPush(&status);
    TestPeek(&status);
    TestIsEmpty(&status);
    TestPop(&status);

    PrintSummary(&status);
    return 0;
}

void TestStackCreate(StatusType* status)
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

void TestPush(StatusType* status)
{
    Stack* s = NULL;
    int num_items = 50;
    int item_size = 1;
    char item = 0;

    s = StackCreate(num_items, item_size);
    PrepareForTest("Test a push by top index update", status);
    item = '{';
    StackPush(s, (void*)&item);
    CheckResult(s->top_index == 0, __LINE__, status);

    PrepareForTest("Next Push", status);
    StackPush(s, (void*)&item);
    CheckResult(s->top_index == 1, __LINE__, status);

    StackDestroy(s);
}

void TestPeek(StatusType* status)
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
        if(i == num_items - 2)
            item = special_top;
        else
            item = '{';
        StackPush(s, &item);    
    }

    StackPeek(s, &output);
    CheckResult(special_top == output, __LINE__, status);
    PrepareForTest("Top index is stack size", status);
    CheckResult(s->top_index + 1 == s->stack_size, __LINE__, status);

    StackDestroy(s);
}

void TestIsEmpty(StatusType* status)
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

    PrepareForTest("Not empty after push", status);
    StackPush(s, &item);
    CheckResult(!StackIsEmpty(s), __LINE__, status);

    StackDestroy(s);
}

void TestPop(StatusType* status)
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
        CheckResult(FAIL, __LINE__, status);
    }

    StackDestroy(s);
}

void PrepareForTest(char *desc, StatusType* status)
{
    printf("test number %d:\n", ++status->test_num);
    printf("    %s  \n", desc);
}

void CheckResult(int condition, int line_of_failure, StatusType* status)
{
    if(condition)
    {
        printf("Test PASS!!\n\n");
        ++status->num_pass;
    }
    else
    {
        printf("Test FAIL at %d :(\n\n", line_of_failure);
        ++status->num_fail;
    }
}

void PrintSummary(StatusType* status)
{
    printf("\n\n    SUMMARY:\n\n");
    printf("Num of tests pass: %d\\%d\n", status->num_pass, status->test_num);
    printf("Num of tests fail: %d\\%d\n", status->num_fail, status->test_num);
    printf("fin.\n");
}

