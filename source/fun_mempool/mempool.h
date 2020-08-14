#ifndef __MEMPOOL_H
#define __MEMPOOL_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 *  this is a mempool implement
 *  free-list -|     --------------------
               |->   |     |             |
               |-    --------------------
               |     --------------------
               |     |                   |  -> for client use
               |     --------------------
               |     --------------------
               |->   |     |             |
                     --------------------
 */

enum { __ALIGN = 8 };
enum { __MAX_BYTES = 128};
enum { __NFREELISTS = __MAX_BYTES/__ALIGN }; //free-lists 16个

typedef union obj{
    union obj *free_list_link; //下一个节点的指针 指向下一个obj (空闲链表)
    char client_data[1]; //obj分配出去时，使用
}obj;
//-------------------------------------

typedef struct mem_pool_manager_t{
    obj *volatile free_list[__NFREELISTS]; //free list for mem
    char *start_free;       //mempool start position  内存池的首地址
    char *end_free;         //mempoll end position    内存池的尾地址
    size_t heap_size; //内存池大小
}mem_pool_manager;
//-------------------------------------


void *chunk_alloc(mem_pool_manager *pmanager,size_t size,int *nobjs);


void *refill(mem_pool_manager *pmanager,size_t n);



//-------------------------------------------------
// for use outside
//

bool mem_pool_init(mem_pool_manager *pmanager);


void *allocate(mem_pool_manager *pmanager,size_t n);



void deallocate(mem_pool_manager *pmanager,void *p,size_t n);


#ifdef __cplusplus
}
#endif

#endif
