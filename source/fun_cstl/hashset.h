#ifndef __HASHSET_H
#define __HASHSET_H

#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>

#include "list.h"

#if defined(__cplusplus) && (__cplusplus > 0)
extern "C" {
#endif

typedef struct {
    unsigned int capacity, size;
    struct hlist_head *hashtable;

    // Basic methods
    int (*make_key)(uintptr_t *key);
    int (*free_key)(uintptr_t key);
    int (*compare_key)(uintptr_t key1, uintptr_t key2);       // non-NULL required
    int (*calculate_hash)(uintptr_t key, unsigned int *hash); // non-NULL required

    // Variable Arguments methods
    int (*emplace_key)(uintptr_t *key, va_list ap);
} hashset_t;
typedef struct hlist_node * hashset_iterator_t;

int hashset_init(hashset_t *set, unsigned int capacity);
int hashset_deinit(hashset_t *set);

int hashset_size(const hashset_t *set);
int hashset_empty(const hashset_t *set);
int hashset_contains(const hashset_t *set, uintptr_t key);

int hashset_insert(hashset_t *set, uintptr_t key);
int hashset_remove(hashset_t *set, uintptr_t key);
int hashset_clear(hashset_t *set);

int hashset_emplace(hashset_t *set, ...);

#if defined(__cplusplus) && (__cplusplus > 0)
}
#endif

#endif /* __HASHTABLE_H */
