#ifndef __KV_LIST_H
#define __KV_LIST_H

#include "avl.h"

#ifdef __cplusplus
extern "C" {
#endif


struct kvlist {
    struct avl_tree avl;

    int (*get_len)(struct kvlist *kv, const void *data);
};

struct kvlist_node {
    struct avl_node avl;

    char data[0] __attribute__((aligned(4)));
};

#define KVLIST_INIT(_name, _get_len)						\
    {									\
        .avl = AVL_TREE_INIT(_name.avl, avl_strcmp, false, NULL),	\
        .get_len = _get_len						\
    }

#define KVLIST(_name, _get_len)							\
    struct kvlist _name = KVLIST_INIT(_name, _get_len)

#define __ptr_to_kv(_ptr) container_of(((char *) (_ptr)), struct kvlist_node, data[0])
#define __avl_list_to_kv(_l) container_of(_l, struct kvlist_node, avl.list)

#define kvlist_for_each(kv, name, value) \
    for (value = (void *) __avl_list_to_kv((kv)->avl.list_head.next)->data,			\
         name = (const char *) __ptr_to_kv(value)->avl.key, (void) name;			\
         &__ptr_to_kv(value)->avl.list != &(kv)->avl.list_head;				\
         value = (void *) (__avl_list_to_kv(__ptr_to_kv(value)->avl.list.next))->data,	\
         name = (const char *) __ptr_to_kv(value)->avl.key)

void kvlist_init(struct kvlist *kv, int (*get_len)(struct kvlist *kv, const void *data));
void kvlist_free(struct kvlist *kv);
void *kvlist_get(struct kvlist *kv, const char *name);
bool kvlist_set(struct kvlist *kv, const char *name, const void *data);
bool kvlist_delete(struct kvlist *kv, const char *name);

#ifdef __cplusplus
}
#endif

#endif

