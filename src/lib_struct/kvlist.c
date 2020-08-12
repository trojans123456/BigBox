#include <stdlib.h>
#include <string.h>

#include "alloc.h"
#include "kvlist.h"

static int avl_strcmp(const void *k1, const void *k2, void *ptr)
{
    return strcmp(k1, k2);
}

void kvlist_init(struct kvlist *kv, int (*get_len)(struct kvlist *kv, const void *data))
{
    avl_init(&kv->avl, avl_strcmp, false, NULL);
    kv->get_len = get_len;
}

static struct kvlist_node *__kvlist_get(struct kvlist *kv, const char *name)
{
    struct kvlist_node *node;

    return avl_find_element(&kv->avl, name, node, avl);
}

void *kvlist_get(struct kvlist *kv, const char *name)
{
    struct kvlist_node *node;

    node = __kvlist_get(kv, name);
    if (!node)
        return NULL;

    return node->data;
}

bool kvlist_delete(struct kvlist *kv, const char *name)
{
    struct kvlist_node *node;

    node = __kvlist_get(kv, name);
    if (node) {
        avl_delete(&kv->avl, &node->avl);
        free(node);
    }

    return !!node;
}

bool kvlist_set(struct kvlist *kv, const char *name, const void *data)
{
    struct kvlist_node *node;
    char *name_buf;
    int len = kv->get_len(kv, data);

    node = calloc_a(sizeof(struct kvlist_node) + len,
        &name_buf, strlen(name) + 1);
    if (!node)
        return false;

    kvlist_delete(kv, name);

    memcpy(node->data, data, len);

    node->avl.key = strcpy(name_buf, name);
    avl_insert(&kv->avl, &node->avl);

    return true;
}

void kvlist_free(struct kvlist *kv)
{
    struct kvlist_node *node, *tmp;

    avl_remove_all_elements(&kv->avl, node, avl, tmp)
        free(node);
}
