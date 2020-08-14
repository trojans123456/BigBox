#ifndef ___HASHMAP_H
#define ___HASHMAP_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    unsigned int capacity, size;
    struct hlist_head *hashtable;

    // Basic methods
    int (*make_pair)(void **key, void **value);
    int (*free_pair)(void *key, void *value);
    int (*compare_key)(void *key1, void *key2);       // non-NULL required
    int (*calculate_hash)(uintptr_t key, unsigned int *hash);
} hashmap_t;
typedef struct hlist_node * hashmap_iterator_t;

int hashmap_init(hashmap_t *map, unsigned int capacity);
int hashmap_deinit(hashmap_t *map);

int hashmap_size(const hashmap_t *map);
int hashmap_empty(const hashmap_t *map);
int hashmap_lookup(const hashmap_t *map, void *key, void  **value);

int hashmap_insert(hashmap_t *map, void * key, void *value);
int hashmap_remove(hashmap_t *map, void * key);
int hashmap_clear(hashmap_t *map);

#ifdef __cplusplus
}
#endif

#endif // ___HASHMAP_H
