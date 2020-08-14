#ifndef ___QUEUE_H
#define ___QUEUE_H

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>

#include "deque.h"

#ifdef __cplusplus
extern "C" {
#endif

#define queue_t             deque_t
#define queue_iterator_t    deque_iterator_t
#define queue_new           deque_new
#define queeu_delete        deque_delete
#define queue_empty         deque_empty
#define queue_size          deque_size
#define queue_push          deque_push_back
#define queue_front         deque_pop_front
#define queue_erase         deque_erase_front

#ifdef __cplusplus
}
#endif

#endif // ___QUEUE_H
