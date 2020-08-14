#ifndef ___STACK_H
#define ___STACK_H

#include "deque.h"

#ifdef __cplusplus
extern "C" {
#endif

#define stack_t             deque_t
#define stack_iterator_t    deque_iterator_t
#define stack_new           deque_new
#define stack_destroy       deque_destroy
#define stack_push          deque_push_back
#define stack_pop           deque_pop_back


#define stack_empty         deque_empty
#define stack_size          deque_size
#define stack_front         deque_front
#define stack_back          deque_back


#ifdef __cplusplus
}
#endif

#endif // ___STACK_H
