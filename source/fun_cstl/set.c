#include "alloc_p.h"
#include "rbtree_p.h"
#include "set.h"

struct set_node
{
    void *key;
    struct rbtree_node rb_node;
};

struct __set
{
    size_t size;
    size_t typesize;
    set_ops_t *set_ops;
    struct rbtree_root rb_root;
};

#define set_check(expr) (assert(expr))

set_t *set_new(size_t typesize,set_ops_t *ops)
{
    set_t *a_this = NULL;
    if(typesize <= 0)
        return NULL;
    a_this = Calloc(1,sizeof(set_t));
    set_check(a_this);

    a_this->rb_root.rb_node = NULL;
    a_this->size = 0;
    a_this->typesize = typesize;
    a_this->set_ops = ops;

    return a_this;
}
void set_delete(set_t *a_this)
{
    set_check(a_this);
    set_clear(a_this);
}

size_t set_size(const set_t *a_this)
{
    set_check(a_this);
    return a_this->size;
}
bool set_empty(const set_t *a_this)
{
    return set_size(a_this);
}


static struct set_node *lookup(const set_t *set, void *key)
{
    struct rbtree_node *node = set->rb_root.rb_node;

    while (node != NULL) {
        struct set_node *tmp = rbtree_entry(node, struct set_node, rb_node);
        int ret = set->set_ops->compare_key(key, tmp->key);
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


/*ÅÐ¶Ï¸ÃkeyÊÇ·ñ´æÔÚ*/
bool set_contains(const set_t *a_this,void *key)
{
    if(a_this == NULL || a_this->set_ops->compare_key == NULL)
        return false;
    return lookup(a_this,key) != NULL;
}

int set_insert(set_t *a_this,void *key)
{
    if(a_this == NULL ||
       a_this->set_ops == NULL ||
       a_this->set_ops->compare_key == NULL)
    {
        return -1;
    }

    int err = 0;
    struct rbtree_node **ptmp = &(a_this->rb_root.rb_node), *parent = NULL;

    /* Figure out where to put new node */
    while (*ptmp) {
        struct set_node *tmp = rbtree_entry(*ptmp, struct set_node, rb_node);
        int result = a_this->set_ops->compare_key(key, tmp->key);
        parent = *ptmp;
        if (result < 0) {
            ptmp = &((*ptmp)->lchild);
        } else if (result > 0) {
            ptmp = &((*ptmp)->rchild);
        } else {
            return -1;
        }
    }
    struct set_node *node = (struct set_node *)Calloc(1, sizeof(struct set_node));
    if (node == NULL) {
        return -1;
    }

    if (a_this->set_ops && a_this->set_ops->make_key)
    {
        a_this->set_ops->make_key(&node->key);

        memcpy(node->key,key,a_this->typesize);
    }
    else
    {
        node->key = key;
    }
    rbtree_init_node(&node->rb_node);

    /* Add new node and rebalance tree. */
    rbtree_link_node(&node->rb_node, parent, ptmp);
    rbtree_insert_color(&node->rb_node, &(a_this->rb_root));
    a_this->size++;

    return 0;
}
int set_remove(set_t *set,void *key)
{
    if (set == NULL || set->set_ops->compare_key == NULL) {
        return -1;
    }
    struct set_node *node = lookup(set, key);
    if (node == NULL) {
        return -1;
    }
    rbtree_erase(&(node->rb_node), &set->rb_root);
    if (set->set_ops->free_key != NULL) {
        set->set_ops->free_key(node->key);
    }
    Free(node);
    set->size--;

    return 0;
}


static void clear_helper(struct rbtree_node *node, set_t *set)
{
    if (node == NULL) {
        return;
    }
    if (node->lchild) {
        clear_helper(node->lchild, set);
        node->lchild = NULL;
    }
    if (node->rchild) {
        clear_helper(node->rchild, set);
        node->rchild = NULL;
    }
    struct set_node *entry = rbtree_entry(node, struct set_node, rb_node);
    if (set->set_ops->free_key != NULL) {
        set->set_ops->free_key(entry->key);
    }
    Free(entry);
}

int set_clear(set_t *set)
{
    if(!set)
        return -1;
    clear_helper(set->rb_root.rb_node, set);
    set->rb_root.rb_node = NULL;
    return 0;
}
