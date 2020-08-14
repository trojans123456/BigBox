#ifndef __MINHEAP_H
#define __MINHEAP_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


struct minheap;

struct minHeapNode
{
    int32_t key;
    void    *data;
};


struct minheap{
    struct minHeapNode  *nodeList;
    int32_t             headIndex;
    int32_t             lastIndex;
    int32_t             maxNum;

    int32_t (* insert)(struct minheap *this, int32_t key, void *data);
    int32_t (* pop)(struct minheap *this,  struct minHeapNode *node_);
    int32_t (* des)(struct minheap *this);

};

struct minheap * minHeapInit(int32_t maxNum);

#ifdef __cplusplus
}
#endif

#endif
