#ifndef ___MAP_H
#define ___MAP_H

#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>

#include "rbtree_p.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct
{
    bool (*make_pair)(char **key,size_t *key_len,void **value,size_t *val_len);
    int (*free_pair)(char *key,void *value);
    int (*compare_key)(const char *key1,const char *key2);
}map_ops_t;

typedef struct __map    map_t;
//typedef struct rbtree_node * map_iterator_t;


map_t *map_new(map_ops_t *ops);
void map_delete(map_t *a_this);

size_t map_size(const map_t *a_this);
bool map_empty(const map_t *a_this);

void *map_find(const map_t *a_this, char *key);

int map_insert(map_t *map, char *key, void *value);
int map_remove(map_t *map, char *key);
int map_clear(map_t *map);



#if defined(__cplusplus)
}
#endif


#endif // ___MAP_H
