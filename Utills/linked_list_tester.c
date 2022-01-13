#include <stdio.h>  /* printf */
#include <stdlib.h> /* free */

#include "tester.h"
#include "node.h"
#include "linked_list.h"

static void PrintChar(void *data);

void TestListCreate(TestStatusType*);
void TestListPush(TestStatusType*);
void TestListRemove(TestStatusType*);
void TestListFind(TestStatusType*);

TestFunction tests[] = {TestListCreate, TestListPush, TestListFind, TestListRemove};

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

void TestListPush(TestStatusType* status)
{
    SLinkedList* list = NULL;
    int size = 1;
    char data[] = {"string"};
    SNode *node = NULL;

    node = NodeCreate(size, &data[1], NULL);

    PrintTestSubject("LIST PUSH AND APPEND");
    list = LinkListCreate(NULL, size);
    PrepareForTest("Test a push using append to empty list", status);
    LinkListAppend(list, node);
    CheckResult(LinkListSize(list) == 1, __LINE__, status);

    PrepareForTest("Next Push", status);
    node = NodeCreate(size, &data[0], NULL);
    LinkListPush(list, node);
    CheckResult(LinkListSize(list) == 2, __LINE__, status);

    LinkListPrint(list, PrintChar);
    printf("\n");

    PrepareForTest("Append rest of \"string\"", status);
    node = NodeCreate(size, &data[2], NULL);
    LinkListAppend(list, node);
    node = NodeCreate(size, &data[3], NodeCreate(size, &data[4], 
            NodeCreate(size, &data[5], NULL)));
    LinkListAppend(list, node);
    CheckResult(LinkListSize(list) == (sizeof(data) - 1), __LINE__, status);

    LinkListPrint(list, PrintChar);
    printf("\n\n");
    LinkListDestroy(list);
}

void TestListRemove(TestStatusType* status)
{
    SLinkedList* list = NULL;
    int size = 1;
    char data[] = {"string"};
    SNode *node = NULL;

    PrintTestSubject("LIST REMOVE");
    PrepareForTest("Create a list and remove middle", status);
    list = LinkListCreate(NodeCreate(size, &data[0], 
                          NodeCreate(size, &data[1], 
                          NodeCreate(size, &data[2], 
                          NodeCreate(size, &data[3], 
                          NodeCreate(size, &data[4],
                          NodeCreate(size, &data[5], NULL)))))), size);

    node = LinkListFind(list, &data[2]);
    LinkListRemove(node);
    CheckResult(LinkListSize(list) == sizeof(data) - 2 && 
        LinkListFind(list, &data[2]) == NULL, __LINE__, status);

    LinkListPrint(list, PrintChar);

    PrepareForTest("Create a list and remove middle", status);
    node = LinkListFind(list, &data[5]);
    LinkListRemove(node);
    CheckResult(LinkListSize(list) == sizeof(data) - 3 &&
        LinkListFind(list, &data[5]) == NULL, __LINE__, status);
        
    LinkListDestroy(list);
}

void TestListFind(TestStatusType* status)
{
     SLinkedList* list = NULL;
    int size = 1;
    char data[] = {"string"};
    SNode *node = NULL;

    PrintTestSubject("LIST FIND");
    PrepareForTest("Create a list and search for NULL", status);
    list = LinkListCreate(NodeCreate(size, &data[0], 
                          NodeCreate(size, &data[1], 
                          NodeCreate(size, &data[2], 
                          NodeCreate(size, &data[3], 
                          NodeCreate(size, &data[4],
                          NodeCreate(size, &data[5], NULL)))))), size);
    
    /*LinkListPrint(list, PrintChar);*/
    node = LinkListFind(list, NULL);
    CheckResult(node == NULL, __LINE__, status);

    PrepareForTest("Search for middle letter from string and compare", status);
    node = LinkListFind(list, &data[2]);
    CheckResult(*(char *)node->data == data[2], __LINE__, status);

    PrepareForTest("Search for last letter from string and compare", status);
    node = LinkListFind(list, &data[5]);
    CheckResult(*(char *)node->data == data[5], __LINE__, status);

    LinkListDestroy(list);
}

void PrintChar(void *data)
{
    printf("| %c |", *(char *)data);
}