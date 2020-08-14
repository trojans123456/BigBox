#ifndef __MAP_H
#define __MAP_H


#ifdef __cplusplus
extern "C" {
#endif


typedef struct map map_t;
typedef struct rb_root root_t;
typedef struct rb_node rb_node_t;

map_t *get(root_t *root, char *str);

int put(root_t *root, char* key, void* val,unsigned long val_size);

map_t *map_first(root_t *tree);

map_t *map_next(rb_node_t *node);

void map_free(map_t *node);

#ifdef __cplusplus
}
#endif

#endif
