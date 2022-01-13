#ifndef __LINKED_LIST_H__
#define __LINKED_LIST_H__

#include "node.h"

typedef struct _SLinkedList SLinkedList;

typedef void (*PrintFunc)(void *);

/* create a new list containing 'node' as it's head */
SLinkedList* LinkListCreate(SNode *node, size_t data_size);

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

/* get node at index */
SNode* LinkListGetAt(SLinkedList *list, int index);

/* if not found return NULL */
SNode* LinkListFind(SLinkedList *list, void *data);

/* print list using a costume user defined function */
void LinkListPrint(SLinkedList *list, PrintFunc func);

#endif /* __LINKED_LIST_H__ */