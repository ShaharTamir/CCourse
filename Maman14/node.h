#ifndef __NODE_H__
#define __NODE_H__

typedef struct SNode
{
    void *data;
    struct SNode *next;
} SNode;

/* is node is not allocated properly - return NULL */
SNode* NodeCreate(void *data, SNode *next);

/* release allocated memory */
void NodeDestroy(SNode *node);

#endif /* __NODE_H__ */