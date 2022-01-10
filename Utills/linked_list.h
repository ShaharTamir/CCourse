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

/* count starts from 0 */
void LinkListRemoveAt(SLinkedList *list, unsigned int index);

/* remove the first node from list */
void LinkListPop(SLinkedList *list);

/*void LinkListPrint(SLinkedList *list, PrintFunc func);*/

/* return num of nodes in list */
int LinkListSize(SLinkedList *list);

/* get node at index */
SNode* LinkListGetAt(SLinkedList *list, int index);

/* if not found return NULL */
SNode* LinkListFind(SLinkedList *list, void *data);

#endif /* __LINKED_LIST_H__ */