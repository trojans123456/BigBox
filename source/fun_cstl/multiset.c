#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "rbtree_p.h"
#include "multiset.h"

struct multiset_node {
    uintptr_t key;
    int count;
    struct rbtree_node rb_node;
};
static struct multiset_node *lookup(const multiset_t *set, uintptr_t key);

int multiset_init(multiset_t *set)
{
    if (set == NULL) {
        return -EINVAL;
    }
    set->rb_root.rb_node = NULL;
    set->make_key = NULL;
    set->free_key = NULL;
    set->compare_key = NULL;
    set->size = 0;

    return 0;
}

int multiset_deinit(multiset_t *set)
{
    if (set == NULL) {
        return -EINVAL;
    }
    multiset_clear(set);

    return 0;
}

int multiset_insert(multiset_t *set, uintptr_t key)
{
    if (set == NULL || set->compare_key == NULL) {
        return -EINVAL;
    }
    struct rbtree_node **prb = &(set->rb_root.rb_node), *parent = NULL;

    /* Figure out where to put new node */
    while (*prb) {
        struct multiset_node *tmp = rbtree_entry(*prb, struct multiset_node, rb_node);
        int result = set->compare_key(key, tmp->key);
        parent = *prb;
        if (result < 0) {
            prb = &((*prb)->lchild);
        } else if (result > 0) {
            prb = &((*prb)->rchild);
        } else {
            tmp->count++;
            return 0;
        }
    }
    struct multiset_node *node = (struct multiset_node *)calloc(1, sizeof(struct multiset_node));
    if (node == NULL) {
        return -ENOMEM;
    }
    int err = 0;
    node->key = key;
    if (set->make_key != NULL && (err = set->make_key(&node->key)) != 0) {
        free(node);
        return err;
    }
    rbtree_init_node(&node->rb_node);
    node->count = 0;

    /* Add new node and rebalance tree. */
    rbtree_link_node(&node->rb_node, parent, prb);
    rbtree_insert_color(&node->rb_node, &(set->rb_root));
    set->size++;

    return 0;
}

int multiset_remove(multiset_t *set, uintptr_t key)
{
    if (set == NULL || set->compare_key == NULL) {
        return -EINVAL;
    }
    struct multiset_node *node = lookup(set, key);
    if (node == NULL) {
        return -ENOENT;
    }
    node->count--;
    if (node->count <= 0) {
        rbtree_erase(&(node->rb_node), &set->rb_root);
        if (set->free_key != NULL) {
            set->free_key(node->key);
        }
        free(node);
        set->size--;
    }

    return 0;
}

int multiset_remove_all(multiset_t *set, uintptr_t key)
{
    if (set == NULL || set->compare_key == NULL) {
        return -EINVAL;
    }
    struct multiset_node *node = lookup(set, key);
    if (node == NULL) {
        return -ENOENT;
    }
    rbtree_erase(&(node->rb_node), &set->rb_root);
    if (set->free_key != NULL) {
        set->free_key(node->key);
    }
    free(node);
    set->size--;

    return 0;
}

static void clear_helper(struct rbtree_node *node, multiset_t *set)
{
    if (node == NULL) {
        return;
    }
    if (node->lchild != NULL) {
        clear_helper(node->lchild, set);
        node->lchild = NULL;
    }
    if (node->rchild != NULL) {
        clear_helper(node->rchild, set);
        node->rchild = NULL;
    }
    struct multiset_node *entry = rbtree_entry(node, struct multiset_node, rb_node);
    if (set->free_key != NULL) {
        set->free_key(entry->key);
    }
    free(entry);
}

int multiset_clear(multiset_t *set)
{
    if (set == NULL) {
        return -EINVAL;
    }
    clear_helper(set->rb_root.rb_node, set);
    set->rb_root.rb_node = NULL;

    return 0;
}

int multiset_size(const multiset_t *set)
{
    if (set == NULL) {
        return 0;
    }
    return set->size;
}

int multiset_empty(const multiset_t *set)
{
    return multiset_size(set) == 0;
}

int multiset_contains(const multiset_t *set, uintptr_t key)
{
    if (set == NULL || set->compare_key == NULL) {
        return 0;
    }
    return lookup(set, key) != NULL;
}

int multiset_count(const multiset_t *set, uintptr_t key)
{
    if (set == NULL || set->compare_key == NULL) {
        return 0;
    }
    struct multiset_node *node = lookup(set, key);
    return node ? node->count : 0;
}

int multiset_emplace(multiset_t *set, ...)
{
    if (set == NULL || set->emplace_key == NULL) {
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

    if ((err = multiset_insert(set, key)) != 0) {
        set->free_key(key);
        return err;
    }
    return 0;
}

static struct multiset_node *lookup(const multiset_t *set, uintptr_t key)
{
    struct rbtree_node *node = set->rb_root.rb_node;

    while (node) {
        struct multiset_node *tmp = rbtree_entry(node, struct multiset_node, rb_node);
        int ret = set->compare_key(key, tmp->key);
        if (ret < 0) {
            node = node->lchild;
        } else if (ret > 0) {
            node = node->rchild;
        } else {
            return tmp;
        }
    }
    return NULL;
}
