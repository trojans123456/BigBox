#ifndef ___LIST_H
#define ___LIST_H

#include <stddef.h>
#include <stdbool.h>

#include "list_p.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    void *(*list_alloc_obj)(void *);
    void (*list_free_obj)(void *);
}list_ops_t;

typedef struct __list list_t;
typedef struct list_head * list_iterator_t;

list_t *list_new(size_t typesize,list_ops_t *ops);
void list_delete(list_t *a_this);

bool empty_list(list_t *a_this);
size_t size_list(list_t *a_this);

list_iterator_t list_push_front(list_t *a_this,void *data);
list_iterator_t list_push_back(list_t *a_this,void *data);
list_iterator_t list_insert(list_t *a_this,list_iterator_t pos,void *data);

void *list_pop_front(list_t *a_this);
void *list_pop_back(list_t *a_this);

list_iterator_t list_insert_before(list_t *a_this,list_iterator_t pos,void *data);

list_iterator_t list_erase(list_t *a_this,list_iterator_t it);
list_iterator_t list_erase_front(list_t *a_this);
list_iterator_t list_erase_back(list_t *a_this);

list_iterator_t list_find(list_t *a_this,void *data);

/* ·­×ª */
void reverse_list(list_t *a_this);

void list_clear(list_t *a_this);

/*ºÏ²¢ */
list_t *list_merge(list_t *list1,list_t *list2);

#ifdef __cplusplus
}
#endif // __cplusplus


#endif // ___LIST_H
