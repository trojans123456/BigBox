#include <stdlib.h>

#include "minheap.h"



struct minheap * minHeapInit(int32_t maxNum){
    if(maxNum <= 0){
        return (struct minheap *)NULL;
    }

    struct minheap * a_this = (struct minheap *)malloc(sizeof(struct minheap));
    if(a_this == (struct minheap *)NULL){
        return (struct minheap *)NULL;
    }

    a_this->nodeList = \
        (struct minHeapNode *)calloc(maxNum, sizeof(struct minHeapNode));
    if(a_this->nodeList == NULL){
        free(a_this);
        return (struct minheap *)NULL;
    }

    a_this->headIndex = 1;
    a_this->lastIndex = 1;
    a_this->maxNum    = maxNum;

    a_this->insert = &_minHeapInsert;
    a_this->pop    = &_minHeapPop;
    a_this->des    = &_minHeapDes;
    return (struct minheap *)a_this;
}

int32_t _getFatherIndex(int32_t index){
    if(index == 1 || index == 0){
        return 0;
    }

    return (int)(index / 2);
}

int32_t _getLeftChildIndex(int32_t index){
    if(index == 0){
        return 0;
    }
    return index*2;
}

int32_t _getRightChildIndex(int32_t index){
    if(index == 0){
        return 0;
    }
    return index*2 + 1;
}

void _exchange(int32_t *i1, int32_t *i2){
    int32_t temp = *i1;
    *i1 = *i2;
    *i2 = temp;
}

void _exchangePoint(void **app, void **bpp)
{
    void *temp = *app;
    *app = *bpp;
    *bpp = temp;
}

int32_t _minHeapInsert(struct minheap *a_this,
                                     int32_t         key,
                                     void           *data){

    if(a_this->lastIndex >= a_this->maxNum-1){
        return -1;
    }
    int32_t index = a_this->lastIndex;

    a_this->nodeList[a_this->lastIndex].data = data;
    a_this->nodeList[a_this->lastIndex].key  = key;

    a_this->lastIndex += 1;

    int32_t fatherIndex = _getFatherIndex(index);
    //int32_t temp = -1;
    while(fatherIndex){
        if(key < a_this->nodeList[fatherIndex].key){
            _exchange(&a_this->nodeList[fatherIndex].key, \
                      &a_this->nodeList[index].key);
            _exchangePoint(                             \
                &a_this->nodeList[fatherIndex].data,      \
                &a_this->nodeList[index].data);

            index       = fatherIndex;
            fatherIndex = _getFatherIndex(fatherIndex);
        }
        else
            break;
    }

    return 0;
}

static inline int32_t _minChildIndex(struct minheap *a_this, int32_t index){
    if( index >= a_this->lastIndex ){
        return 0;
    }
    int32_t _l = _getLeftChildIndex(index);
    int32_t _r = _getRightChildIndex(index);

    if( _l >= a_this->lastIndex){
        return 0;
    }else if( _r >= a_this->lastIndex ){
        return _l;
    }

    return a_this->nodeList[_l].key < a_this->nodeList[_r].key ? _l : _r ;
}

int32_t _minHeapPop(struct minheap *a_this, struct minHeapNode *node){
    if(a_this->lastIndex == 1){
        return -1;
    }

    node->key  = a_this->nodeList[a_this->headIndex].key;
    node->data = a_this->nodeList[a_this->headIndex].data;

    _exchange(&a_this->nodeList[a_this->lastIndex-1].key, \
              &a_this->nodeList[a_this->headIndex].key);

    _exchangePoint(&a_this->nodeList[a_this->lastIndex-1].data,  \
                   &a_this->nodeList[a_this->headIndex].data);

    a_this->lastIndex -= 1;

    int32_t index      = a_this->headIndex;
    int32_t childIndex = _minChildIndex(a_this, index);

    while(childIndex){
        if(a_this->nodeList[index].key >= a_this->nodeList[childIndex].key){
            _exchange(&a_this->nodeList[index].key, \
                      &a_this->nodeList[childIndex].key);
            _exchangePoint(&a_this->nodeList[index].data, \
                          &a_this->nodeList[childIndex].data);

            index      = childIndex;
            childIndex = _minChildIndex(a_this, childIndex);
        }else{
            break;
        }
    }

    return 0;
}

int32_t _minHeapDes(struct minheap *a_this){
    if(a_this == (struct minheap *)NULL){
        return -1;
    }
    if(a_this->nodeList != NULL){
        free(a_this->nodeList);
        a_this->nodeList = NULL;
    }
    free(a_this);
    return 0;
}
