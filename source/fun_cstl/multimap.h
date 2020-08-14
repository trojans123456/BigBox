#ifndef ___MULTIMAP_H
#define ___MULTIMAP_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    int (*make_pair)(char **key,void **value);
    int (*free_pair)(char *key,void *value);
    int (*compare_key)(const char *key1,const char *key2);
    int (*compare_value)(const void *value1,const void *value2);
}multimap_ops_t;

typedef struct __multimap multimap_t;

multimap_t *multimap_new(multimap_ops_t *ops);
void multimap_delete(multimap_t *a_this);


int multimap_size(const multimap_t *map);
int multimap_empty(const multimap_t *map);
struct list_head *multimap_lookup(const multimap_t *map, void * key);

int multimap_insert(multimap_t *map, void * key, void * value);
int multimap_remove(multimap_t *map, void * key, void * value);
int multimap_remove_all(multimap_t *map, void * key);
int multimap_clear(multimap_t *map);

#ifdef __cplusplus
}
#endif

#endif // ___MULTIMAP_H
