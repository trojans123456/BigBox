#ifndef ___PRIO_QUEUE_H
#define ___PRIO_QUEUE_H

#include <stddef.h>
#include <stdbool.h>

#include "list_p.h"

#ifdef __cplusplus
extern "C" {
#endif

/* priority queue */

typedef struct
{
    void *(*prio_queue_alloc_obj)(void *);
    void (*prio_queue_free_obj)(void *);
    int (*prio_queue_compare_obj)(const void *obj1,const void *obj2);
}prio_queue_ops_t;

typedef struct list_head *prio_queue_iterator_t;
typedef struct __prio_queue prio_queue_t;

prio_queue_t *prio_queue_new(int typesize,prio_queue_ops_t *ops);
void prio_queue_delete(prio_queue_t *a_this);

bool prio_queue_empty(prio_queue_t *a_this);
size_t prio_queue_size(prio_queue_t *a_this);

void *prio_queue_pop_front(prio_queue_t *a_this);
void *prio_queue_pop_back(prio_queue_t *a_this);

prio_queue_iterator_t prio_queue_push(prio_queue_t *a_this,void *data);

prio_queue_iterator_t prio_queue_erase(prio_queue_t *a_this,prio_queue_iterator_t pos);

#ifdef __cplusplus
}
#endif

#endif // ___PRIO_QUEUE_H
