#ifndef ___RBTREE_P_H
#define ___RBTREE_P_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


#if !defined(offsetof)
#define offsetof(TYPE, MEMBER) ((size_t) & ((TYPE *)0)->MEMBER)
#endif

#if !defined(container_of)
#define container_of(ptr, type, member) ((type *)(void *)((char *)(ptr) - offsetof(type,member) ))
#endif


#if defined(__cplusplus)
extern "C" {
#endif

#define RB_RED 0
#define RB_BLACK 1
struct rbtree_node {
    uintptr_t parent_color;
    struct rbtree_node *rchild;
    struct rbtree_node *lchild;
} __attribute__((aligned(sizeof(void *))));

static const uintptr_t RBTREE_MASK_PARENT = (~((uintptr_t)0) >> 2) << 2;
static const uintptr_t RBTREE_MASK_COLOR = (uintptr_t)0x1;

struct rbtree_root {
    struct rbtree_node *rb_node;
};


#define rbtree_parent(r)    ((struct rbtree_node *)(uintptr_t)((r)->parent_color & RBTREE_MASK_PARENT))
#define rbtree_color(r)     (unsigned int)((r)->parent_color & RBTREE_MASK_COLOR)
#define rbtree_is_red(r)    !rbtree_color(r)
#define rbtree_is_black(r)  rbtree_color(r)
#define rbtree_set_red(r)   do { (r)->parent_color &= ~RBTREE_MASK_COLOR; } while (0)
#define rbtree_set_black(r) do { (r)->parent_color |= RBTREE_MASK_COLOR; } while (0)


static inline void rbtree_set_parent(struct rbtree_node *rb, struct rbtree_node *p)
{
    rb->parent_color = (rb->parent_color & RBTREE_MASK_COLOR) | (uintptr_t)p;
}
static inline void rbtree_set_color(struct rbtree_node *rb, unsigned int color)
{
    rb->parent_color = ((uintptr_t)rb->parent_color & ~RBTREE_MASK_COLOR) | (uintptr_t)color;
}

#define rbtree_entry(ptr, type, member) container_of(ptr, type, member)

#define RB_EMPTY_ROOT(root) ((root)->rb_node == NULL)
#define RB_EMPTY_NODE(node) (rbtree_parent(node) == (node))
#define RB_CLEAR_NODE(node) (rbtree_set_parent(node, node))

static inline void rbtree_init_node(struct rbtree_node *rb)
{
    rb->parent_color = 0;
    rb->rchild = NULL;
    rb->lchild = NULL;
    RB_CLEAR_NODE(rb);
}

void rbtree_insert_color(struct rbtree_node *, struct rbtree_root *);
void rbtree_erase(struct rbtree_node *, struct rbtree_root *);

/* Find logical next and previous nodes in a tree */
struct rbtree_node *rbtree_next(struct rbtree_node *);
struct rbtree_node *rbtree_prev(struct rbtree_node *);
struct rbtree_node *rbtree_first(struct rbtree_root *);
struct rbtree_node *rbtree_last(struct rbtree_root *);

/* Fast replacement of a single node without remove/rebalance/add/rebalance */
void rbtree_replace_node(struct rbtree_node *victim, struct rbtree_node *_new, struct rbtree_root *root);

static inline void rbtree_link_node(struct rbtree_node *node, struct rbtree_node *parent,
                                    struct rbtree_node **rbtree_link)
{
    node->parent_color = (uintptr_t)parent;
    node->lchild = node->rchild = NULL;

    *rbtree_link = node;
}

#if defined(__cplusplus)
}
#endif


#endif // ___RBTREE_P_H
