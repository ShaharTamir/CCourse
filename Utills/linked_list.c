#include <stdio.h>
#include <stdlib.h> /* malloc, free */
#include <string.h> /* memcmp */
#include "node.h"
#include "linked_list.h"

struct _SLinkedList
{
    SNode *head;
    CompareFunc cmp_func;
};

SLinkedList* LinkListCreate(SNode *node, CompareFunc cmp_func)
{
    SLinkedList *new_list = NULL;
    
    if(cmp_func == NULL)
        return NULL;

    new_list = (SLinkedList*)malloc(sizeof(SLinkedList));

    if(new_list)
    {
        new_list->head = NodeCreate(NULL, NULL);
        
        if(new_list->head)
        {
            new_list->cmp_func = cmp_func;
            LinkListPush(new_list, node);
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
        }
        
        free(list->head);
        list->head = NULL;
        list->cmp_func = NULL;

        free(list);
        list = NULL;
    }
}

void LinkListPush(SLinkedList *list, SNode *node)
{
    SNode *iter = NULL;

    if(list && node)
    {
        iter = node;
        while(iter->next)
        {
            iter = iter->next;
        }

        iter->next = list->head;
        list->head = node;
    }
}

void LinkListAppend(SLinkedList *list, SNode *node)
{
    SNode *iter = NULL;
    SNode *node_iter = NULL;
    if(list && node)
    {

        if(list->head->next)
        {
            iter = list->head;
            
            while(iter->next && iter->next->data) /* run to last node until behind tail (dummy) */
            {
                iter = iter->next;
            }

            node_iter = node;
            while(node_iter->next)
            {
                node_iter = node_iter->next;
            }

            node_iter->next = iter->next; /* attach last of node to tail (dummy) */
            iter->next = node; /* attach node behind tail to the beggining of new node part */
        }
        else
        {
            LinkListPush(list, node);
        }
    }
}

void LinkListRemove(SNode* node)
{
    SNode temp;

    if(node)
    {
        /* if not dummy - swap and remove */
        if(node->next != NULL)
        {
            temp.data = node->data;
            temp.next = node->next;

            node->data = node->next->data;
            node->next->data = temp.data;
            node->next = node->next->next;

            /* temp.next containing pointer to allocated SNode */
            NodeDestroy(temp.next);
            temp.next = NULL;
        }
    }
}

void LinkListPop(SLinkedList *list)
{
    LinkListRemove(list->head);
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

SNode* LinkListFind(SLinkedList *list, void *data, void *params)
{
    SNode *iter = NULL;
    
    if(list && data)
    {
       iter = list->head;

       while(iter->next != NULL)
       {
           if(0 == list->cmp_func(iter->data, data, params))
           {
               return iter;
           }

           iter = iter->next;
       }
    }

    return NULL;
}

SNode* LinkListForEach(SLinkedList *list, ActionFunc action, void *params)
{
    SNode *iter = NULL;

    if(list)
    {
        iter = list->head;
        while(iter->next) /* not tail */
        {
            if(!action(iter->data, params))
                return iter;

            iter = iter->next;        
        }
    }

    return NULL;
}

void LinkListPrint(SLinkedList *list, PrintFunc print)
{
    SNode *iter = NULL;

    if(list)
    {
        iter = list->head;
        while(iter->next) /* not tail */
        {
            print(iter->data);
            iter = iter->next;
        }
    }
}

SNode *LinkListGetTail(SLinkedList *list)
{
    SNode *iter = NULL;
    SNode *node_iter = NULL;
    
    if(list && list->head->next)
    {
        iter = list->head;
            
        while(iter->next && iter->next->data) /* run to last node until behind tail (dummy) */
        {
            iter = iter->next;
        }
    }

    return iter;   
}