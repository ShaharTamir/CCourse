#include <stdio.h>
#include <stdlib.h> /* malloc, free */
#include <string.h> /* memcmp */
#include "node.h"
#include "linked_list.h"

struct _SLinkedList
{
    SNode *head;
};

SLinkedList* LinkListCreate(SNode *node, size_t data_size)
{
    SLinkedList *new_list = NULL;

    if(0 == data_size)
        return NULL;
        
    new_list = (SLinkedList*)malloc(sizeof(SLinkedList));

    if(new_list)
    {
        new_list->head = (SNode*)malloc(sizeof(SNode));
        
        if(new_list->head)
        {
            new_list->head->data = NULL; /* tail is dummy */
            new_list->head->next = node;
            new_list->head->size = data_size; /* have to receive this if create empty list */
        }
        else
        {
            free(new_list);
            new_list = NULL;
        }
    }

    return new_list;
}

void LinkListDestroy(SLinkedList *list)
{
    if(list)
    {
        while(list->head->next)
        {
            LinkListPop(list);
            printf("still popping\n");
        }
        
        free(list->head);
        list->head = NULL;

        free(list);
        list = NULL;
    }
}

void LinkListPush(SLinkedList *list, SNode *node)
{
    if(list && node)
    {
        node->next = list->head->next;
        list->head->next = node;
    }
}

void LinkListRemoveAt(SLinkedList *list, unsigned int index)
{
    int i = 0;
    SNode *iter = NULL;
    SNode temp;

    if(list)
    {
        iter = list->head;
        for(i = 0; i < index; ++i)
        {
            if(iter->next == NULL)
                return;

            iter = iter->next;
        }

        /* if not dummy - swap and remove */
        if(iter->next != NULL)
        {
            temp.data = iter->data;
            temp.next = iter->next;

            iter->data = iter->next->data;
            iter->next->data = temp.data;
            iter->next = iter->next->next;

            /* temp.next containing pointer to allocated SNode */
            NodeDestroy(temp.next);
            printf("destoryed\n");
        }
    }
}

void LinkListPop(SLinkedList *list)
{
    LinkListRemoveAt(list, 0);
}

int LinkListSize(SLinkedList *list)
{
    SNode *iter = NULL;
    int count = 0;

    if(list)
    {
        iter = list->head;
        while(iter->next != NULL)
        {
            ++count;
            iter = iter->next;
        }    
    }

    return count;
}

SNode* LinkListFind(SLinkedList *list, void *data)
{
    SNode *iter = NULL;
    
    if(list)
    {
       iter = list->head;

       while(iter->next != NULL)
       {
           if(0 == memcmp(iter->data, data, list->head->size))
           {
               return iter;
           }
       }
    }

    return NULL;
}