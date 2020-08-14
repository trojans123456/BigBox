#include "alloc_p.h"
#include "list_p.h"
#include "rbtree_p.h"
#include "multimap.h"

struct multimap_node
{
    void *value;
    struct list_head head;
};

struct multimap_list
{
    void *key;
    int count;
    struct list_head head;
    struct rbtree_node rb_node;
};

struct __multimap
{
    size_t size;
    struct rbtree_root rb_root;
    multimap_ops_t *ops;
};

#define multimap_check(expr) (assert(expr))

multimap_t *multimap_new(multimap_ops_t *ops)
{
    multimap_t *a_this = NULL;

    a_this = Calloc(1,sizeof(multimap_t));
    multimap_check(a_this);

    a_this->size = 0;
    a_this->rb_root.rb_node = NULL;
    a_this->ops = ops;

    return a_this;
}
void multimap_delete(multimap_t *a_this)
{
    multimap_check(a_this);
    multimap_clear(a_this);
}


static struct multimap_list *lookup(const multimap_t *map, void * key)
{
    struct rbtree_node *node = map->rb_root.rb_node;
    while (node != NULL) {
        struct multimap_list *tmp = rbtree_entry(node, struct multimap_list, rb_node);
        int ret = map->ops->compare_key(key, tmp->key);
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

int multimap_insert(multimap_t *map, void * key, void * value)
{
    if (map == NULL || map->ops->compare_key == NULL || map->ops->compare_value == NULL) {
        return -1;
    }
    struct multimap_node *node = (struct multimap_node *)Calloc(1, sizeof(struct multimap_node));
    if (node == NULL) {
        return -1;
    }
    int err = 0;
    node->value = value;
    if (map->ops != NULL && map->ops->make_pair)
    {
        map->ops->make_pair(NULL, &node->value);
        /* copy ?? */
    }
    INIT_LIST_HEAD(&node->head);

    struct rbtree_node **ptmp = &(map->rb_root.rb_node), *parent = NULL;

    /* Figure out where to put new node */
    while (*ptmp) {
        struct multimap_list *tmp = rbtree_entry(*ptmp, struct multimap_list, rb_node);
        int result = map->ops->compare_key(key, tmp->key);
        parent = *ptmp;
        if (result < 0) {
            ptmp = &((*ptmp)->lchild);
        } else if (result > 0) {
            ptmp = &((*ptmp)->rchild);
        } else {
            list_add(&tmp->head, &node->head);
            tmp->count++;
            return 0;
        }
    }

    struct multimap_list *list = (struct multimap_list *)Calloc(1, sizeof(struct multimap_list));
    if (list == NULL) {
        free(node);
        return -1;
    }
    list->key = key;
    if (map->ops->make_pair != NULL && (err = map->ops->make_pair(&list->key, NULL)) != 0) {
        if (map->ops->free_pair != NULL) {
            map->ops->free_pair((void *)0, node->value);
        }
        free(node);
        free(list);
        return err;
    }
    rbtree_init_node(&list->rb_node);

    /* Add new node and rebalance tree. */
    rbtree_link_node(&list->rb_node, parent, ptmp);
    rbtree_insert_color(&list->rb_node, &(map->rb_root));
    map->size++;

    return 0;
}

int multimap_remove(multimap_t *map, void * key, void * value)
{
    if (map == NULL || map->ops->compare_key == NULL || map->ops->compare_value == NULL) {
        return -1;
    }
    struct multimap_list *list = lookup(map, key);
    if (list == NULL) {
        return -1;
    }
    struct multimap_node *node = NULL;
    list_for_each_entry (node, &list->head, head, struct multimap_node) {
        if (map->ops->compare_value(node->value, value) == 0) {
            list_del(&node->head);
            if (map->ops->free_pair != NULL) {
                map->ops->free_pair((void *)0, node->value);
            }
            free(node);
            if (list_empty(&list->head)) {
                rbtree_erase(&(list->rb_node), &map->rb_root);
                if (map->ops->free_pair != NULL) {
                    map->ops->free_pair(list->key, (void *)0);
                }
                Free(list);
                map->size--;
            }
        }
    }

    return 0;
}

int multimap_remove_all(multimap_t *map, void * key)
{
    if (map == NULL || map->ops->compare_key == NULL) {
        return -1;
    }
    struct multimap_list *list = lookup(map, key);
    if (list == NULL) {
        return -1;
    }
    struct multimap_node *node = NULL;
    list_for_each_entry (node, &list->head, head, struct multimap_node) {
        list_del(&node->head);
        if (map->ops->free_pair != NULL) {
            map->ops->free_pair((void *)0, node->value);
        }
        free(node);
    }
    rbtree_erase(&(list->rb_node), &map->rb_root);
    if (map->ops->free_pair != NULL) {
        map->ops->free_pair(list->key, (void *)0);
    }
    free(list);
    map->size--;

    return 0;
}

struct list_head *multimap_lookup(const multimap_t *map, void * key)
{
    if (map == NULL || map->ops->compare_key == NULL || map->ops->compare_value == NULL) {
        return NULL;
    }
    struct multimap_list *list = lookup(map, key);
    if (list == NULL) {
        return NULL;
    }
    return &list->head;
}

static void clear_helper(struct rbtree_node *node, multimap_t *map)
{
    if (node == NULL) {
        return;
    }
    if (node->lchild != NULL) {
        clear_helper(node->lchild, map);
        node->lchild = NULL;
    }
    if (node->rchild != NULL) {
        clear_helper(node->rchild, map);
        node->rchild = NULL;
    }
    struct multimap_list *list = rbtree_entry(node, struct multimap_list, rb_node);
    struct multimap_node *tmp = NULL;
    list_for_each_entry (tmp, &list->head, head, struct multimap_node) {
        list_del(&tmp->head);
        if (map->ops->free_pair != NULL) {
            map->ops->free_pair((void *)0, tmp->value);
        }
        free(node);
    }
    rbtree_erase(&(list->rb_node), &map->rb_root);
    if (map->ops->free_pair != NULL) {
        map->ops->free_pair(list->key, (void *)0);
    }
    free(list);
}

int multimap_clear(multimap_t *map)
{
    if (map == NULL) {
        return -1;
    }
    clear_helper(map->rb_root.rb_node, map);
    map->rb_root.rb_node = NULL;

    return 0;
}

int multimap_size(const multimap_t *map)
{
    if (map == NULL) {
        return 0;
    }
    return map->size;
}

int multimap_empty(const multimap_t *map)
{
    return multimap_size(map) == 0;
}




