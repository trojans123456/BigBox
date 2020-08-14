#ifndef ___SET_H
#define ___SET_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>



#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    int (*make_key)(void **key);
    int (*free_key)(void *key);
    int (*compare_key)(const void *key1,const void *key2);
}set_ops_t;

typedef struct __set set_t;

set_t *set_new(size_t typesize,set_ops_t *ops);
void set_delete(set_t *a_this);

size_t set_size(const set_t *a_this);
bool set_empty(const set_t *a_this);
/*≈–∂œ∏√key «∑Ò¥Ê‘⁄*/
bool set_contains(const set_t *a_this,void *key);

int set_insert(set_t *a_this,void *key);
int set_remove(set_t *a_this,void *key);
int set_clear(set_t *a_this);



#ifdef __cplusplus
}
#endif

#endif // ___SET_H
