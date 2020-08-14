#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "list.h"
#include "hashset.h"

struct hashset_node {
    uintptr_t key;
    struct hlist_node head;
};
static struct hashset_node *lookup(const hashset_t *set, uintptr_t key);

int hashset_init(hashset_t *set, unsigned int capacity)
{
    if (set == NULL) {
        return -EINVAL;
    }
    set->hashtable = (struct hlist_head *)calloc(1, sizeof(struct hlist_head) * capacity);
    if (set->hashtable == NULL) {
        return -ENOMEM;
    }
    memset(set->hashtable, 0, sizeof(struct hlist_head) * capacity);
    for (unsigned int i = 0; i < capacity; i++) {
        INIT_HLIST_HEAD(&set->hashtable[i]);
    }
    set->capacity = capacity;
    set->make_key = NULL;
    set->free_key = NULL;
    set->compare_key = NULL;
    set->calculate_hash = NULL;
    set->emplace_key = NULL;
    set->size = 0;

    return 0;
}

int hashset_deinit(hashset_t *set)
{
    if (set == NULL) {
        return -EINVAL;
    }
    hashset_clear(set);
    free(set->hashtable);
    set->hashtable = NULL;

    return 0;
}

int hashset_insert(hashset_t *set, uintptr_t key)
{
    if (set == NULL || set->calculate_hash == NULL || set->compare_key == NULL) {
        return -EINVAL;
    }
    unsigned int hash = 0;
    if (set->calculate_hash(key, &hash) != 0) {
        return -EINVAL;
    }
    hash %= set->capacity;

    struct hashset_node *node = NULL;
    // Check conflict
    if (!hlist_empty(&set->hashtable[hash])) {
        hlist_for_each_entry (node, &set->hashtable[hash], head, struct hashset_node) {
            if (set->compare_key(node->key, key) == 0) {
                return -EEXIST;
            }
        }
    }

    int err = 0;
    node = (struct hashset_node *)calloc(1, sizeof(struct hashset_node));
    if (node == NULL) {
        return -ENOMEM;
    }
    node->key = key;
    if (set->make_key != NULL && (err = set->make_key(&node->key)) != 0) {
        free(node);
        return err;
    }
    INIT_HLIST_NODE(&node->head);
    hlist_add_head(&node->head, &set->hashtable[hash]);
    set->size++;

    return 0;
}

int hashset_remove(hashset_t *set, uintptr_t key)
{
    if (set == NULL || set->calculate_hash == NULL || set->compare_key == NULL) {
        return -EINVAL;
    }
    struct hashset_node *node = lookup(set, key);
    if (node != NULL) {
        hlist_del(&node->head);
        if (set->free_key != NULL) {
            set->free_key(node->key);
        }
        free(node);
        set->size--;
        return 0;
    }
    return -ENOENT;
}

int hashset_clear(hashset_t *set)
{
    if (set == NULL) {
        return -EINVAL;
    }
    for (unsigned int i = 0; i < set->capacity; i++) {
        struct hashset_node *node = NULL;
        struct hlist_node *hlist = NULL;
        hlist_for_each_entry_safe (node, hlist, &set->hashtable[i], head, struct hashset_node) {
            hlist_del(&node->head);
            if (set->free_key != NULL) {
                set->free_key(node->key);
            }
            free(node);
        }
    }
    set->size = 0;
    return 0;
}

int hashset_size(const hashset_t *set)
{
    if (set == NULL) {
        return 0;
    }
    return (int)set->size;
}

int hashset_empty(const hashset_t *set)
{
    return hashset_size(set) == 0;
}

int hashset_contains(const hashset_t *set, uintptr_t key)
{
    if (set == NULL || set->calculate_hash == NULL || set->compare_key == NULL) {
        return 0;
    }
    return lookup(set, key) != NULL;
}

int hashset_emplace(hashset_t *set, ...)
{
    if (set == NULL || set->calculate_hash == NULL || set->compare_key == NULL || set->emplace_key == NULL) {
        return -EINVAL;
    }
    int err = 0;
    uintptr_t key = 0;

    va_list ap;
    va_start(ap, set);
    if ((err = set->emplace_key(&key, ap)) != 0) {
        va_end(ap);
        return err;
    }
    va_end(ap);

    if ((err = hashset_insert(set, key)) != 0) {
        if (set->free_key != NULL) {
            set->free_key(key);
        }
        return err;
    }
    return 0;
}

static struct hashset_node *lookup(const hashset_t *set, uintptr_t key)
{
    unsigned int hash = 0;
    if (set->calculate_hash(key, &hash) != 0) {
        return NULL;
    }
    hash %= set->capacity;
    if (hlist_empty(&set->hashtable[hash])) {
        // data not exist
        return NULL;
    } else {
        struct hashset_node *node = NULL;
        hlist_for_each_entry (node, &set->hashtable[hash], head, struct hashset_node) {
            if (set->compare_key(node->key, key) == 0) {
                return node;
            }
        }
        return NULL;
    }
}
