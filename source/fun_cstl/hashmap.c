
#include "list.h"
#include "hashmap.h"

struct hashmap_node {
    void *key;
    void *value;
    struct hlist_node head;
};
static struct hashmap_node *lookup(const hashmap_t *map, void * key);

int hashmap_init(hashmap_t *map, unsigned int capacity)
{
    if (map == NULL || capacity == 0) {
        return -1;
    }
    map->hashtable = (struct hlist_head *)calloc(1, sizeof(struct hlist_head) * capacity);
    if (map->hashtable == NULL) {
        return -1;
    }
    unsigned int i;
    for (i = 0; i < capacity; i++) {
        INIT_HLIST_HEAD(&map->hashtable[i]);
    }
    map->capacity = capacity;
    map->make_pair = NULL;
    map->free_pair = NULL;
    map->compare_key = NULL;
    map->calculate_hash = NULL;

    map->size = 0;

    return 0;
}

int hashmap_deinit(hashmap_t *map)
{
    if (map == NULL) {
        return -1;
    }
    hashmap_clear(map);
    free(map->hashtable);
    map->hashtable = NULL;

    return 0;
}

int hashmap_insert(hashmap_t *map, void * key, void * value)
{
    if (map == NULL || map->calculate_hash == NULL || map->compare_key == NULL) {
        return -1;
    }
    unsigned int hash = 0;
    if (map->calculate_hash(key, &hash) != 0) {
        return -1;
    }
    hash %= map->capacity;

    struct hashmap_node *node = NULL;
    // Check conflictions
    if (!hlist_empty(&map->hashtable[hash])) {
        hlist_for_each_entry (node, &map->hashtable[hash], head, struct hashmap_node) {
            if (map->compare_key(node->key, key) == 0) {
                return -1;
            }
        }
    }

    int err = 0;
    node = (struct hashmap_node *)calloc(1, sizeof(struct hashmap_node));
    if (node == NULL) {
        return -1;
    }
    node->key = key;
    node->value = value;
    if (map->make_pair != NULL && (err = map->make_pair(&node->key, &node->value)) != 0) {
        free(node);
        return err;
    }
    INIT_HLIST_NODE(&node->head);
    hlist_add_head(&node->head, &map->hashtable[hash]);
    map->size++;

    return 0;
}

int hashmap_remove(hashmap_t *map, void * key)
{
    if (map == NULL || map->calculate_hash == NULL || map->compare_key == NULL) {
        return -1;
    }
    struct hashmap_node *node = lookup(map, key);
    if (node != NULL) {
        hlist_del(&node->head);
        if (map->free_pair != NULL) {
            map->free_pair(node->key, node->value);
        }
        free(node);
        map->size--;
        return 0;
    }
    return -1;
}

int hashmap_clear(hashmap_t *map)
{
    if (map == NULL || map->hashtable == NULL) {
        return -1;
    }
    unsigned int i;
    for (i = 0; i < map->capacity; i++) {
        struct hashmap_node *node = NULL;
        struct hlist_node *hlist = NULL;
        hlist_for_each_entry_safe (node, hlist, &map->hashtable[i], head, struct hashmap_node) {
            hlist_del(&node->head);
            if (map->free_pair != NULL) {
                map->free_pair(node->key, node->value);
            }
            free(node);
        }
    }
    return 0;
}

int hashmap_size(const hashmap_t *map)
{
    if (map == NULL) {
        return 0;
    }
    return (int)map->size;
}

int hashmap_empty(const hashmap_t *map)
{
    return hashmap_size(map) == 0;
}

int hashmap_lookup(const hashmap_t *map, void * key, void **value)
{
    if (map == NULL || map->calculate_hash == NULL || map->compare_key == NULL) {
        return -1;
    }
    struct hashmap_node *node = lookup(map, key);
    if (node == NULL) {
        return -1;
    }
    if (value != NULL) {
        *value = &node->value;
    }
    return 0;
}


static struct hashmap_node *lookup(const hashmap_t *map, void * key)
{
    unsigned int hash = 0;
    if (map->calculate_hash(key, &hash) != 0) {
        return NULL;
    }
    hash %= map->capacity;
    if (hlist_empty(&map->hashtable[hash])) {
        // data not exist
        return NULL;
    } else {
        struct hashmap_node *node = NULL;
        hlist_for_each_entry (node, &map->hashtable[hash], head, struct hashmap_node) {
            if (map->compare_key(node->key, key) == 0) {
                return node;
            }
        }
        return NULL;
    }
}

