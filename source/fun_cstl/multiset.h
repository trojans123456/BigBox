#ifndef __MULTISET_H
#define __MULTISET_H

#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>

#include "rbtree_p.h"

#if defined(__cplusplus) && (__cplusplus > 0)
extern "C" {
#endif

typedef struct {
    int size;
    struct rbtree_root rb_root;

    // Basic methods
    int (*make_key)(uintptr_t *key);
    int (*free_key)(uintptr_t key);
    int (*compare_key)(uintptr_t key1, uintptr_t key2);   // non-NULL required

    // Variable Arguments methods
    int (*emplace_key)(uintptr_t *key, va_list ap);
} multiset_t;
typedef struct rbtree_node * multiset_iterator_t;

int multiset_init(multiset_t *set);
int multiset_deinit(multiset_t *set);

int multiset_size(const multiset_t *set);
int multiset_empty(const multiset_t *set);
int multiset_contains(const multiset_t *set, uintptr_t key);
int multiset_count(const multiset_t *set, uintptr_t key);

int multiset_insert(multiset_t *set, uintptr_t key);
int multiset_remove(multiset_t *set, uintptr_t key);
int multiset_remove_all(multiset_t *set, uintptr_t key);
int multiset_clear(multiset_t *set);

int multiset_emplace(multiset_t *set, ...);

#if defined(__cplusplus) && (__cplusplus > 0)
}
#endif

#endif
