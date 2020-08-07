#ifndef __MIN_HEAP_H
#define __MIN_HEAP_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


struct minHeapNode{
    int32_t key   ;
    void    *data ;
};

struct minheap{
    struct minHeapNode  *nodeList;
    int32_t             headIndex;
    int32_t             lastIndex;
    int32_t             maxNum;

    int32_t (* insert)(struct minheap *a_this, int32_t key, void *data);
    int32_t (* pop)(struct minheap *a_this,  struct minHeapNode *node_);
    int32_t (* des)(struct minheap *a_this);

};

int32_t _minHeapInsert(struct minheap *a_this,int32_t key, void *data);

int32_t _minHeapPop(struct minheap *a_this,struct minHeapNode *node_);

int32_t _minHeapDes(struct minheap *a_this);

struct minheap * minHeapInit(int32_t maxNum);





#ifdef __cplusplus
}
#endif

#endif
