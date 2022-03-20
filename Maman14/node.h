#ifndef __NODE_H__
#define __NODE_H__

typedef struct SNode
{
    void *data;
    struct SNode *next;
} SNode;

SNode* NodeCreate(void *data, SNode *next);

void NodeDestroy(SNode *node);

#endif /* __NODE_H__ */