#ifndef ___DEQUE_H
#define ___DEQUE_H

#include <stddef.h>
#include <stdbool.h>

#include "list_p.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    void *(*deque_alloc_obj)(void *);
    void (*deque_free_obj)(void *);
}deque_ops_t;

typedef struct __deque deque_t;
typedef struct list_head * deque_iterator_t;

deque_t *deque_new(int typesize,deque_ops_t *ops);
void deque_delete(deque_t *a_this);

bool deque_empty(deque_t *a_this);
size_t deque_size(deque_t *a_this);

deque_iterator_t deque_push_front(deque_t *a_this,void *data);
deque_iterator_t deque_push_back(deque_t *a_this,void *data);

void *deque_pop_front(deque_t *a_this);
void *deque_pop_back(deque_t *a_this);

deque_iterator_t deque_erase(deque_t *a_this,deque_iterator_t pos);
deque_iterator_t deque_erase_front(deque_t *a_this);
deque_iterator_t deque_erase_back(deque_t *a_this);

deque_iterator_t deque_clear(deque_t *a_this);


#ifdef __cplusplus
}
#endif

#endif // ___DEQUE_H
