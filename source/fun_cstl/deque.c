#include "alloc_p.h"
#include "deque.h"

struct deque_node
{
    void *data;
    struct list_head head;
};

struct __deque
{
    size_t size;
    size_t typesize;
    deque_ops_t *deque_ops;
    struct list_head head;
};

#define deque_check(expr)  (assert(expr))

deque_t *deque_new(int typesize,deque_ops_t *ops)
{
    struct __deque *a_this = NULL;

    if(typesize <= 0)
        return NULL;

    a_this = (struct __deque *)Calloc(1,sizeof(struct __deque));
    deque_check(a_this);

    INIT_LIST_HEAD(&a_this->head);
    a_this->size = 0;
    a_this->typesize = typesize;
    a_this->deque_ops = ops;

    return a_this;
}

void deque_delete(deque_t *a_this)
{
    deque_check(a_this);
    /* del all node */
    struct deque_node *node,*n;
    list_for_each_entry_safe(node,n,&a_this->head,head,struct deque_node)
    {
        list_del(&node->head);/* del node from list */
        if(a_this->deque_ops->deque_free_obj)
        {
            a_this->deque_ops->deque_free_obj(node->data); /* del data */
        }
        free(node);
    }
    a_this->size = 0;
    Free(a_this);
}

bool deque_empty(deque_t *a_this)
{
    deque_check(a_this);
    return (list_empty(&a_this->head) == 1);
}
size_t deque_size(deque_t *a_this)
{
    deque_check(a_this);
    return a_this->size;
}


static deque_iterator_t deque_insert(deque_t *a_this,deque_iterator_t pos,void *data)
{
    if(!a_this || !pos)
        return NULL;

    struct deque_node *node = (struct deque_node *)Calloc(1,sizeof(struct deque_node));
    if(!node)
        return NULL;

    if(a_this->deque_ops && a_this->deque_ops->deque_alloc_obj)
    {

        node->data = a_this->deque_ops->deque_alloc_obj(data);
        deque_check(node->data);
        memcpy(node->data,data,a_this->typesize);
    }
    else
    {
        node->data = data;
    }

    list_add(&node->head,pos);
    a_this->size++;

    return &node->head;
}

deque_iterator_t deque_push_front(deque_t *a_this,void *data)
{
    return deque_insert(a_this,&a_this->head,data);
}
deque_iterator_t deque_push_back(deque_t *a_this,void *data)
{
    return deque_insert(a_this,a_this->head.prev,data);
}

void *deque_pop_front(deque_t *a_this)
{
    deque_check(a_this);
    if(list_empty(&a_this->head))
        return NULL;

    struct deque_node *node = list_first_entry(&a_this->head,struct deque_node,head);
    return node->data;
}
void *deque_pop_back(deque_t *a_this)
{
    deque_check(a_this);
    if(list_empty(&a_this->head))
        return NULL;

    struct deque_node *node = list_last_entry(&a_this->head,struct deque_node,head);
    return node->data;
}

deque_iterator_t deque_erase(deque_t *a_this,deque_iterator_t it)
{
    deque_check(a_this);

    deque_iterator_t next = it->next;
    list_del(it);
    a_this->size--;

    struct deque_node *node = list_entry(it,struct deque_node,head);
    if(a_this->deque_ops->deque_free_obj)
    {
        a_this->deque_ops->deque_free_obj(node->data);
    }
    free(node);

    return next;
}
deque_iterator_t deque_erase_front(deque_t *a_this)
{
    deque_check(a_this);
    if(list_empty(&a_this->head))
        return NULL;

    return deque_erase(a_this,a_this->head.next);
}
deque_iterator_t deque_erase_back(deque_t *a_this)
{
    deque_check(a_this);
    if(list_empty(&a_this->head))
        return NULL;

    return deque_erase(a_this,a_this->head.prev);
}

deque_iterator_t deque_clear(deque_t *a_this)
{
     deque_check(a_this);
    /* del all node */
    struct deque_node *node,*n;
    list_for_each_entry_safe(node,n,&a_this->head,head,struct deque_node)
    {
        list_del(&node->head);/* del node from list */
        if(a_this->deque_ops->deque_free_obj)
        {
            a_this->deque_ops->deque_free_obj(node->data); /* del data */
        }
        free(node);
    }
    a_this->size = 0;
}
