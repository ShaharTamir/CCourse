#include <stdio.h>
#include <stdlib.h> /* malloc, free */
#include <string.h> /* memcpy */
#include "node.h"

SNode *NodeCreate(unsigned int size, void *data, SNode *next)
{
    SNode *new_node = NULL;

    if(size > 0 && data)
    {
        new_node = (SNode*) malloc(sizeof(SNode));
        
        if(new_node)
        {
            new_node->data = malloc(size);

            if(new_node->data)
            {
                new_node->size = size;
                memcpy(new_node->data, data, size);
                new_node->next = next;
            }
            else
            {
                free(new_node);
                new_node = NULL;
            }
        }
    }

    return new_node;
}

void NodeDestroy(SNode *node)
{
    if(node)
    {
        if(node->data)
        {
            free(node->data);
            node->data = NULL;
        }

        node->next = NULL;

        free(node);
        node = NULL;
    }
}