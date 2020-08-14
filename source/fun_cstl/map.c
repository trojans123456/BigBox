#include <assert.h>
#include "alloc_p.h"
#include "map.h"

struct map_node
{
    char *key;
    void *value;
    struct rbtree_node rb_node;
};

struct __map{
    size_t size;
    map_ops_t *map_ops;
    struct rbtree_root rb_root;
};

#define map_check(expr) (assert(expr))

map_t *map_new(map_ops_t *ops)
{
    map_t *a_this = NULL;

    a_this = Calloc(1,sizeof(map_t));
    map_check(a_this);

    a_this->rb_root.rb_node = NULL;
    a_this->size = 0;
    a_this->map_ops = ops != NULL ? ops : NULL;

    return a_this;
}
void map_delete(map_t *a_this)
{
    map_check(a_this);

    map_clear(a_this);
}

size_t map_size(const map_t *a_this)
{
    map_check(a_this);
    return a_this->size;
}
bool map_empty(const map_t *a_this)
{
    return (map_size(a_this) == 0);
}

int map_insert(map_t *map, char *key, void *value)
{
    map_check(map);
    map_check(key);

    bool err = 0;
    struct rbtree_node **prb = &(map->rb_root.rb_node), *parent = NULL;
    /* Figure out where to put new node */
    while (*prb) {
        struct map_node *tmp = rbtree_entry(*prb, struct map_node, rb_node);
        int result = map->map_ops->compare_key(key, tmp->key);
        parent = *prb;
        if (result < 0) {
            prb = &((*prb)->lchild);
        } else if (result > 0) {
            prb = &((*prb)->rchild);
        } else {
            return -1;
        }
    }

    struct map_node *node = (struct map_node *)Calloc(1, sizeof(struct map_node));
    if (node == NULL) {
        return -1;
    }

    node->key = key;
    node->value = value;

    if(map->map_ops && map->map_ops->make_pair)
    {
        size_t key_len = 0,val_len = 0;
        err = map->map_ops->make_pair(&node->key,&key_len,&node->value,&val_len);
        #if 1
        if(err == false)
        {
            Free(node);
            return err;
        }
        #endif
        memcpy(node->key,key,key_len);
        memcpy(node->value,value,val_len);
    }

    rbtree_init_node(&node->rb_node);

    /* Add new node and rebalance tree. */
    rbtree_link_node(&node->rb_node, parent, prb);
    rbtree_insert_color(&node->rb_node, &(map->rb_root));
    map->size++;

    return 0;
}

static struct map_node *lookup(const map_t *map, char *key)
{
    struct rbtree_node *node = map->rb_root.rb_node;

    while (node) {
        struct map_node *tmp = rbtree_entry(node, struct map_node, rb_node);
        if(tmp)
        {

            int ret = map->map_ops->compare_key(key, tmp->key);
            if (ret < 0) {
                node = node->lchild;
            } else if (ret > 0) {
                node = node->rchild;
            } else {
                return tmp;
            }
        }
    }
    return NULL;
}

void *map_find(const map_t *a_this, char *key)
{
    if(a_this == NULL ||
       a_this->map_ops == NULL ||
       a_this->map_ops->compare_key == NULL)
    {
        return NULL;
    }

    struct map_node *node = lookup(a_this,key);
    if(node == NULL)
    {
        //printf("node is empty\n");
        return NULL;
    }
    return node->value;
}

int map_remove(map_t *map, char *key)
{
    if (map == NULL ||
        map->map_ops||
        map->map_ops->compare_key == NULL)
    {
        return -1;
    }
    struct map_node *node = lookup(map, key);
    if (node == NULL) {
        return -1;
    }
    rbtree_erase(&(node->rb_node), &map->rb_root);

    if (map->map_ops->free_pair != NULL) {
        map->map_ops->free_pair(node->key,node->value);
    }
    Free(node);
    map->size--;

    return 0;
}

static void clear_helper(struct rbtree_node *node, map_t *map)
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
    struct map_node *entry = rbtree_entry(node, struct map_node, rb_node);

    if (map->map_ops->free_pair != NULL) {
        map->map_ops->free_pair(entry->key,entry->value);
    }

    Free(entry);
}

int map_clear(map_t *map)
{
    map_check(map);

    clear_helper(map->rb_root.rb_node,map);
    map->rb_root.rb_node = NULL;
    return 0;
}
