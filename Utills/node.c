#include <stdio.h>
#include <stdlib.h> /* malloc, free */
#include <string.h> /* memcpy */
#include "node.h"

SNode *NodeCreate(void *data, SNode *next)
{
    SNode *new_node = NULL;

    new_node = (SNode*) malloc(sizeof(SNode));
    
    if(new_node)
    {
        new_node->data = data;
        new_node->next = next;
    }

    return new_node;
}

void NodeDestroy(SNode *node)
{
    if(node)
    {
        node->data = NULL;
        node->next = NULL;

        free(node);
        node = NULL;
    }
}