#ifndef __LINKED_LIST_H__
#define __LINKED_LIST_H__

#include "node.h"

typedef struct _SLinkedList SLinkedList;

typedef void (*PrintFunc)(void *node_data);
typedef int (*CompareFunc)(void *node_data, void *data, void *params); /* return 0 if equal */
typedef int (*ActionFunc)(void *node_data, void *params); /* return 1 if OK, 0 if not */

/* create a new list containing 'node' as it's head */
SLinkedList* LinkListCreate(SNode *node, CompareFunc cmp_func);

/* release memory properly */
void LinkListDestroy(SLinkedList *list);

/* push a new node to the beginning of the list */
void LinkListPush(SLinkedList *list, SNode *node);

/* append a new node to the end of the list */
void LinkListAppend(SLinkedList *list, SNode *node);

/* remove node from list. If node is not from list - your problem */
void LinkListRemove(SNode *node);

/* remove the first node from list */
void LinkListPop(SLinkedList *list);

/* return num of nodes in list */
int LinkListSize(SLinkedList *list);

/* if not found return NULL */
SNode* LinkListFind(SLinkedList *list, void *data, void *params);

/* if Action fails - return node where failure accured. else, return NULL */
SNode* LinkListForEach(SLinkedList *list, ActionFunc action, void *params);

/* print list using a costume user defined function */
void LinkListPrint(SLinkedList *list, PrintFunc func);

/* get the tail of the list */
SNode* LinkListGetTail(SLinkedList *list);

#endif /* __LINKED_LIST_H__ */