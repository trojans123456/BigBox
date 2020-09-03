#include "alloc_p.h"
#include "list_p.h"

#include "list.h"

struct list_node
{
    void *data;
    struct list_head head;
};

struct __list
{
    size_t size; /* 个数 */
    size_t typesize; /* 数据类型大小 */
    list_ops_t *list_ops;
    struct list_head head;
};


#define list_check(expr)    (assert(expr))

list_t *list_new(size_t typesize,list_ops_t *ops)
{
    struct __list *a_this = NULL;

    if(typesize <= 0)
        return NULL;

    a_this = (struct __list *)Calloc(1,sizeof(struct __list));
    list_check(a_this);

    INIT_LIST_HEAD(&a_this->head);
    a_this->size = 0;
    a_this->typesize = typesize;
    a_this->list_ops = ops;

    return a_this;
}

void list_delete(list_t *a_this)
{
    list_check(a_this);
    /* del all node */
    struct list_node *node,*n;
    list_for_each_entry_safe(node,n,&a_this->head,head,struct list_node)
    {
        list_del(&node->head);/* del node from list */
        if(a_this->list_ops->list_free_obj)
        {
            a_this->list_ops->list_free_obj(node->data); /* del data */
        }
        free(node);
    }
    a_this->size = 0;
    Free(a_this);
}

bool empty_list(list_t *a_this)
{
    list_check(a_this);
    return (list_empty(&a_this->head) == 1);
}
size_t size_list(list_t *a_this)
{
    list_check(a_this);
    return a_this->size;
}

list_iterator_t list_push_front(list_t *a_this,void *data)
{
    return list_insert(a_this,&a_this->head,data);
}

list_iterator_t list_push_back(list_t *a_this,void *data)
{
    return list_insert(a_this,a_this->head.prev,data);
}

list_iterator_t list_insert(list_t *a_this,list_iterator_t pos,void *data)
{
    if(!a_this || !pos)
        return NULL;

    struct list_node *node = (struct list_node *)Calloc(1,sizeof(struct list_node));
    if(!node)
        return NULL;

    if(a_this->list_ops && a_this->list_ops->list_alloc_obj)
    {

        node->data = a_this->list_ops->list_alloc_obj(data);
        list_check(node->data);
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

void *list_pop_front(list_t *a_this)
{
    list_check(a_this);
    if(list_empty(&a_this->head))
        return NULL;

    struct list_node *node = list_first_entry(&a_this->head,struct list_node,head);
    return node->data;
}

void *list_pop_back(list_t *a_this)
{
    list_check(a_this);
    if(list_empty(&a_this->head))
        return NULL;

    struct list_node *node = list_last_entry(&a_this->head,struct list_node,head);
    return node->data;
}

list_iterator_t list_insert_before(list_t *a_this,list_iterator_t pos,void *data)
{
    list_check(a_this);

    struct list_node *node = (struct list_node *)Calloc(1,sizeof(struct list_node));
    if(!node)
        return NULL;

    if(a_this->list_ops && a_this->list_ops->list_alloc_obj)
    {

        node->data = a_this->list_ops->list_alloc_obj(data);
        list_check(node->data);
        memcpy(node->data,data,a_this->typesize);
    }
    else
    {
        node->data = data;
    }

    list_add_tail(&node->head,pos);
    a_this->size++;

    return &node->head;
}

list_iterator_t list_erase(list_t *a_this,list_iterator_t it)
{
    list_check(a_this);

    list_iterator_t next = it->next;
    list_del(it);
    a_this->size--;

    struct list_node *node = list_entry(it,struct list_node,head);
    if(a_this->list_ops->list_free_obj)
    {
        a_this->list_ops->list_free_obj(node->data);
    }
    free(node);

    return next;
}

list_iterator_t list_erase_front(list_t *a_this)
{
    list_check(a_this);
    if(list_empty(&a_this->head))
        return NULL;

    return list_erase(a_this,a_this->head.next);
}
list_iterator_t list_erase_back(list_t *a_this)
{
     list_check(a_this);
    if(list_empty(&a_this->head))
        return NULL;

    return list_erase(a_this,a_this->head.prev);
}

list_iterator_t list_find(list_t *a_this,void *data)
{

}

/* 翻转 */
void reverse_list(list_t *a_this)
{
    /*循环双向链表没必要 */
   
}

void list_clear(list_t *a_this)
{

}

/*合并 */
list_t *list_merge(list_t *list1,list_t *list2)
{

}
