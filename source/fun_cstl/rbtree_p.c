#include "rbtree_p.h"

static void __rbtree_rotate_left(struct rbtree_node *node, struct rbtree_root *root);
static void __rbtree_rotate_right(struct rbtree_node *node, struct rbtree_root *root);
static void __rbtree_erase_color(struct rbtree_node *node, struct rbtree_node *parent, struct rbtree_root *root);

void rbtree_insert_color(struct rbtree_node *node, struct rbtree_root *root)
{
    struct rbtree_node *parent, *gparent;

    while ((parent = rbtree_parent(node)) != NULL && rbtree_is_red(parent)) {
        gparent = rbtree_parent(parent);

        if (parent == gparent->lchild) {
            {
                register struct rbtree_node *uncle = gparent->rchild;
                if (uncle != NULL && rbtree_is_red(uncle)) {
                    rbtree_set_black(uncle);
                    rbtree_set_black(parent);
                    rbtree_set_red(gparent);
                    node = gparent;
                    continue;
                }
            }

            if (parent->rchild == node) {
                register struct rbtree_node *tmp;
                __rbtree_rotate_left(parent, root);
                tmp = parent;
                parent = node;
                node = tmp;
            }

            rbtree_set_black(parent);
            rbtree_set_red(gparent);
            __rbtree_rotate_right(gparent, root);
        } else {
            {
                register struct rbtree_node *uncle = gparent->lchild;
                if (uncle != NULL && rbtree_is_red(uncle)) {
                    rbtree_set_black(uncle);
                    rbtree_set_black(parent);
                    rbtree_set_red(gparent);
                    node = gparent;
                    continue;
                }
            }

            if (parent->lchild == node) {
                register struct rbtree_node *tmp;
                __rbtree_rotate_right(parent, root);
                tmp = parent;
                parent = node;
                node = tmp;
            }

            rbtree_set_black(parent);
            rbtree_set_red(gparent);
            __rbtree_rotate_left(gparent, root);
        }
    }

    rbtree_set_black(root->rb_node);
}

void rbtree_erase(struct rbtree_node *node, struct rbtree_root *root)
{
    struct rbtree_node *child, *parent;
    unsigned int color;

    if (node->lchild == NULL) {
        child = node->rchild;
    } else if (node->rchild == NULL) {
        child = node->lchild;
    } else {
        struct rbtree_node *old = node, *lchild;

        node = node->rchild;
        while ((lchild = node->lchild) != NULL) {
            node = lchild;
        }

        if (rbtree_parent(old) != NULL) {
            if (rbtree_parent(old)->lchild == old) {
                rbtree_parent(old)->lchild = node;
            } else {
                rbtree_parent(old)->rchild = node;
            }
        } else {
            root->rb_node = node;
        }

        child = node->rchild;
        parent = rbtree_parent(node);
        color = rbtree_color(node);

        if (parent == old) {
            parent = node;
        } else {
            if (child != NULL) {
                rbtree_set_parent(child, parent);
            }
            parent->lchild = child;

            node->rchild = old->rchild;
            rbtree_set_parent(old->rchild, node);
        }

        node->parent_color = old->parent_color;
        node->lchild = old->lchild;
        rbtree_set_parent(old->lchild, node);

        goto color;
    }

    parent = rbtree_parent(node);
    color = rbtree_color(node);

    if (child) rbtree_set_parent(child, parent);
    if (parent != NULL) {
        if (parent->lchild == node) {
            parent->lchild = child;
        } else {
            parent->rchild = child;
        }
    } else {
        root->rb_node = child;
    }

color:
    if (color == RB_BLACK) {
        __rbtree_erase_color(child, parent, root);
    }
}

/*
 * This function returns the key node (in sort order) of the tree.
 */
struct rbtree_node *rbtree_first(struct rbtree_root *root)
{
    struct rbtree_node *n;

    n = root->rb_node;
    if (n == NULL) return NULL;
    while (n->lchild != NULL) n = n->lchild;
    return n;
}

struct rbtree_node *rbtree_last(struct rbtree_root *root)
{
    struct rbtree_node *n;

    n = root->rb_node;
    if (n == NULL) return NULL;
    while (n->rchild != NULL) n = n->rchild;
    return n;
}

struct rbtree_node *rbtree_next(struct rbtree_node *node)
{
    struct rbtree_node *parent;

    if (rbtree_parent(node) == node) return NULL;

    /* If we have a right-hand child, go down and then left as far as we can. */
    if (node->rchild != NULL) {
        node = node->rchild;
        while (node->lchild != NULL) node = node->lchild;
        return (struct rbtree_node *)node;
    }

    /* No right-hand children.  Everything down and left is
       smaller than us, so any 'next' node must be in the general
       direction of our parent. Go up the tree; any time the
       ancestor is a right-hand child of its parent, keep going
       up. First time it's a left-hand child of its parent, said
       parent is our 'next' node. */
    while ((parent = rbtree_parent(node)) != NULL && node == parent->rchild) node = parent;

    return parent;
}

struct rbtree_node *rbtree_prev(struct rbtree_node *node)
{
    struct rbtree_node *parent;

    if (rbtree_parent(node) == node) return NULL;

    /* If we have a left-hand child, go down and then right as far
       as we can. */
    if (node->lchild != NULL) {
        node = node->lchild;
        while (node->rchild != NULL) node = node->rchild;
        return (struct rbtree_node *)node;
    }

    /* No left-hand children. Go up till we find an ancestor which
       is a right-hand child of its parent */
    while ((parent = rbtree_parent(node)) != NULL && node == parent->lchild) node = parent;

    return parent;
}

void rbtree_replace_node(struct rbtree_node *victim, struct rbtree_node *_new, struct rbtree_root *root)
{
    struct rbtree_node *parent = rbtree_parent(victim);

    /* Set the surrounding nodes to point to the replacement */
    if (parent != NULL) {
        if (victim == parent->lchild) {
            parent->lchild = _new;
        } else {
            parent->rchild = _new;
        }
    } else {
        root->rb_node = _new;
    }
    if (victim->lchild != NULL) rbtree_set_parent(victim->lchild, _new);
    if (victim->rchild != NULL) rbtree_set_parent(victim->rchild, _new);

    /* Copy the pointers/colour from the victim to the replacement */
    *_new = *victim;
}

static void __rbtree_rotate_left(struct rbtree_node *node, struct rbtree_root *root)
{
    struct rbtree_node *rchild = node->rchild;
    struct rbtree_node *parent = rbtree_parent(node);

    if ((node->rchild = rchild->lchild) != NULL) {
        rbtree_set_parent(rchild->lchild, node);
    }
    rchild->lchild = node;
    rbtree_set_parent(rchild, parent);

    if (parent != NULL) {
        if (node == parent->lchild) {
            parent->lchild = rchild;
        } else {
            parent->rchild = rchild;
        }
    } else {
        root->rb_node = rchild;
    }
    rbtree_set_parent(node, rchild);
}

static void __rbtree_rotate_right(struct rbtree_node *node, struct rbtree_root *root)
{
    struct rbtree_node *lchild = node->lchild;
    struct rbtree_node *parent = rbtree_parent(node);

    if ((node->lchild = lchild->rchild) != NULL) {
        rbtree_set_parent(lchild->rchild, node);
    }
    lchild->rchild = node;
    rbtree_set_parent(lchild, parent);

    if (parent != NULL) {
        if (node == parent->rchild) {
            parent->rchild = lchild;
        } else {
            parent->lchild = lchild;
        }
    } else {
        root->rb_node = lchild;
    }
    rbtree_set_parent(node, lchild);
}

static void __rbtree_erase_color(struct rbtree_node *node, struct rbtree_node *parent, struct rbtree_root *root)
{
    struct rbtree_node *other;

    while ((node == NULL || rbtree_is_black(node)) && node != root->rb_node) {
        if (parent->lchild == node) {
            other = parent->rchild;
            if (rbtree_is_red(other)) {
                rbtree_set_black(other);
                rbtree_set_red(parent);
                __rbtree_rotate_left(parent, root);
                other = parent->rchild;
            }
            if ((other->lchild == NULL || rbtree_is_black(other->lchild))
                && (other->rchild == NULL || rbtree_is_black(other->rchild))) {
                rbtree_set_red(other);
                node = parent;
                parent = rbtree_parent(node);
            } else {
                if (other->rchild == NULL || rbtree_is_black(other->rchild)) {
                    rbtree_set_black(other->lchild);
                    rbtree_set_red(other);
                    __rbtree_rotate_right(other, root);
                    other = parent->rchild;
                }
                rbtree_set_color(other, rbtree_color(parent));
                rbtree_set_black(parent);
                rbtree_set_black(other->rchild);
                __rbtree_rotate_left(parent, root);
                node = root->rb_node;
                break;
            }
        } else {
            other = parent->lchild;
            if (rbtree_is_red(other)) {
                rbtree_set_black(other);
                rbtree_set_red(parent);
                __rbtree_rotate_right(parent, root);
                other = parent->lchild;
            }
            if ((other->lchild == NULL || rbtree_is_black(other->lchild))
                && (other->rchild == NULL || rbtree_is_black(other->rchild))) {
                rbtree_set_red(other);
                node = parent;
                parent = rbtree_parent(node);
            } else {
                if (other->lchild == NULL || rbtree_is_black(other->lchild)) {
                    rbtree_set_black(other->rchild);
                    rbtree_set_red(other);
                    __rbtree_rotate_left(other, root);
                    other = parent->lchild;
                }
                rbtree_set_color(other, rbtree_color(parent));
                rbtree_set_black(parent);
                rbtree_set_black(other->lchild);
                __rbtree_rotate_right(parent, root);
                node = root->rb_node;
                break;
            }
        }
    }
    if (node != NULL) rbtree_set_black(node);
}

