#include <stdio.h>  /* printf */
#include <stdlib.h> /* free */

#include "tester.h"
#include "node.h"
#include "linked_list.h"

void TestListCreate(TestStatusType*);
/*
void TestListPush(TestStatusType*);
void TestListRemove(TestStatusType*);
void TestListSize(TestStatusType*);
void TestListFind(TestStatusType*);
*/
TestFunction tests[] = {TestListCreate/*, TestListPush, TestListRemove, TestListSize, TestListFind*/};

int main()
{
    TestStatusType status = {0, 0, 0};
    
    RunTests(tests, &status, sizeof(tests) / sizeof(TestFunction));
    PrintSummary(&status);

    return 0;
}

void TestListCreate(TestStatusType* status)
{
    SLinkedList* list = NULL;
    int size = 1;
    char data[] = {"strings"};

    PrintTestSubject("CREATE SINGLE LINKED LIST");
    PrepareForTest("Create invalid data size", status);
    list = LinkListCreate(NULL, 0);
    CheckResult((list == NULL), __LINE__, status);

    PrepareForTest("Create empty list", status);
    list = LinkListCreate(NULL, size);
    CheckResult((list != NULL), __LINE__, status);

    LinkListDestroy(list);
    list = NULL;

    PrepareForTest("Create a full list, verify valgrind", status);
    list = LinkListCreate(NodeCreate(size, &data[0], 
                          NodeCreate(size, &data[1], 
                          NodeCreate(size, &data[2], 
                          NodeCreate(size, &data[3], 
                          NodeCreate(size, &data[4],
                          NodeCreate(size, &data[5], NULL)))))), size);
    CheckResult((list != NULL), __LINE__, status);

    LinkListDestroy(list);
    list = NULL;
}
/*
void TestPush(TestStatusType* status)
{
    Stack* s = NULL;
    int num_items = 50;
    int num_small_items = 2;
    int item_size = 1;
    char item = 0;

    PrintTestSubject("STACK PUSH");
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

    PrintTestSubject("STACK PEEK");
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

    PrintTestSubject("STACK IS EMPTY");
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

    PrintTestSubject("STACK POP");
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

*/