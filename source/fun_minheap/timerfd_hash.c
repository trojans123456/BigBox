#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "timerfd_hash.h"

static unsigned int hash_default_ops_string(const char *a_string)
{
    unsigned int h;
    for(h = 0; *a_string != '\0'; a_string++)
    {
        h = *a_string + 31 * h;
    }
    return h;
}

hash_t *hash_new(size_t a_size,const hash_ops_t *a_ops)
{
    const static hash_ops_t default_ops =
    {
        (void *)&hash_default_ops_string,
        (void *)&strcmp,
        0,0,0,0
    };

    hash_t *hash;
    hash_iter_t **tab;

    if(a_size == 0)
        a_size = SD_HASH_DEFAULT_SIZE;
    hash = calloc(1,sizeof(*hash));
    tab = calloc(a_size,sizeof(*tab));

    if(hash == 0 || tab == 0)
    {
        free(hash);
        free(tab);
        return 0;
    }

    hash->nelem = 0;
    hash->size = a_size;
    hash->tab = tab;
    hash->ops = a_ops != 0 ?a_ops : &default_ops;

    return hash;
}

void hash_delete(hash_t *a_this)
{
    hash_clear(a_this);
    free(a_this->tab);
    free(a_this);
}

void hash_clear(hash_t *a_this)
{
    size_t h;
    hash_iter_t *p;
    hash_iter_t *q;

    if(a_this == 0)
        return ;
    for(h = 0;h < a_this->size;h++)
    {
        for(p = a_this->tab[h];p;p = q)
        {
            q = p->next;
            if(a_this->ops->key_free)
            {
                a_this->ops->key_free(p->key);
            }
            if(a_this->ops->data_free)
            {
                a_this->ops->data_free(p->data);
            }
            free(p);
        }
        a_this->tab[h] = 0;
    }
    a_this->nelem = 0;
}
hash_iter_t *hash_lookup(hash_t *a_this,const void *a_key)
{
    int h;
    hash_iter_t *p;
    if(a_this == 0 || a_key == 0)
        return 0;
    h = hindex(a_this->ops->hash(a_key),a_this->size);

    p = a_this->tab[h];
    //printf("h = %d",h);
    for(p = a_this->tab[h];p != (void*)0;p = p->next)
    {
        printf("key = %s p->key = %s\n",a_key,p->key);
        if(a_this->ops->compare(a_key,p->key) == 0)
        {
            return p;
        }
    }
    return 0;
}

static void rehash(hash_t *a_this)
{
    size_t i;
    int h,size;
    hash_iter_t **tab;
    hash_iter_t *p;
    hash_iter_t *q;

    size = SD_HASH_GROWTAB * a_this->size;
    tab = calloc(size,sizeof(*tab));

    if(tab == 0)
        return ;

    for(i = 0;i <a_this->size;i++)
    {
        for(p = a_this->tab[i];p;p = q)
        {
            q = p->next;
            h = hindex(p->hkey,size);
            p->next = tab[h];
            tab[h] = p;
            if(p->next != 0)
            {
                p->next->prev = p;
            }
            p->prev = 0;
        }
    }
    free(a_this->tab);

    a_this->tab = tab;
    a_this->size = size;
}
//查找一个key，如果不存在就添加它
hash_iter_t *hash_lookadd(hash_t *a_this,const void *a_key)
{
    int h;
    hash_iter_t *p;
    if(a_this == 0 || a_key == 0)
        return 0;
    if((p = hash_lookup(a_this,a_key)) != 0)
        return p;

    if((p = calloc(1,sizeof(*p))) == 0)
        return 0;

    if(a_this->ops->key_dup != 0)
    {
        p->key = a_this->ops->key_dup(a_key);
    }
    else
    {
        p->key = (void *)a_key;
    }

    p->hash = a_this;
    p->hkey = a_this->ops->hash(a_key);

    if(a_this->nelem > SD_HASH_FULLTAB * a_this->size)
        rehash(a_this);

    h = hindex(p->hkey,a_this->size);
    p->next = a_this->tab[h];

    a_this->tab[h] = p;
    if(p->next != 0)
    {
        p->next->prev = p;
    }
    a_this->nelem++;
    return p;
}
//添加一个key:value 如果key存在先删除再创建
hash_iter_t *hash_add(hash_t *a_this,const void *a_key,const void *a_data)
{
    hash_iter_t *p;
    if((p = hash_lookadd(a_this,a_key)) == 0)
        return 0;

    if(a_this->ops->data_free != 0)
    {
        a_this->ops->data_free(p->data);
    }
    if(a_this->ops->data_dup != 0)
    {
        p->data = a_this->ops->data_dup(a_data);
    }
    else
    {
        p->data = (void*)a_data;
    }
    return p;
}
void hash_del(hash_t *a_this,const void *a_key)
{
    int h;
    hash_iter_t *p;
    h = hindex(a_this->ops->hash(a_key),a_this->size);

    for(p = a_this->tab[h];p != 0; p = p->next)
    {
        if(a_this->ops->compare(a_key,p->key) == 0)
            break;
    }
    if(p == 0)
        return ;
    hash_iter_del(p);
}

typedef unsigned int (*hash_func_t)(void *a_key,void *a_data,void *a_userdata);
void hash_foreach(hash_t *a_this,hash_func_t a_func,void *a_userdata)
{
    size_t h,ret;
    hash_iter_t *p;
    hash_iter_t *q;

    if(a_this == 0 || a_func == 0)
        return ;

    for(h = 0;h < a_this->size;h++)
    {
        for(p = a_this->tab[h];p != 0;p = q)
        {
            p->foreach = 1;
            ret = (*a_func)(p->key,p->data,a_userdata);
            q = p->next;

            if(p->foreach == 0)
            {
                hash_iter_del(p);
            }
            else
            {
                p->foreach = 0;
            }
            if(ret != 0)
                return ;
        }
    }
}

unsigned int hash_get_nelem(hash_t *a_this)
{
    if(a_this == 0)
        return 0;
    return a_this->nelem;
}
//获取数据个数
unsigned int hash_get_size(hash_t *a_this)
{
    if(a_this == 0)
        return 0;
    return a_this->size;
}

//指向第一个数据
hash_iter_t *hash_begin(hash_t *a_this)
{
    size_t h;
    if(a_this == 0)
        return 0;
    for(h = 0;h < a_this->size;h++)
    {
        if(a_this->tab[h])
            return a_this->tab[h];
    }
    return (void *)0;
}
hash_iter_t *hash_end(hash_t *a_this)
{
    return (void *)0;
}
//迭代器遍历
hash_iter_t *hash_iter_next(hash_iter_t *a_this)
{
    int h;
    size_t i;
    if(a_this == 0)
        return 0;
    if(a_this->next != 0)
        return a_this->next;

    h = hindex(a_this->hkey,a_this->hash->size);
    for(i = h + 1;i < a_this->hash->size;i++)
    {
        if(a_this->hash->tab[i])
        {
            return a_this->hash->tab[i];
        }
    }
    return (void *)0;
}
hash_iter_t *hash_iter_prev(hash_iter_t *a_this)
{
    int h,i;
    hash_iter_t *p;

    if(a_this == 0)
        return 0;
    if(a_this->prev != 0)
        return a_this->prev;

    h = hindex(a_this->hkey,a_this->hash->size);
    for(i = h - 1;i > 0;i--)
    {
        for(p = a_this->hash->tab[i];p;p = p->next)
        {
            if(p->next == 0)
                return p;
        }
    }
    return (void *)0;
}
void hash_iter_del(hash_iter_t *a_this)
{
    if(a_this == 0)
        return ;
    if(a_this->hash->ops->data_free != 0)
    {
        a_this->hash->ops->data_free(a_this->data);
    }
    a_this->data = 0;
    if(a_this->hash->ops->key_free != 0)
    {
        a_this->hash->ops->key_free(a_this->key);
    }
    a_this->key = 0;

    if(a_this->foreach == 1)
    {
        a_this->foreach = 0;
        return ;
    }
    if(a_this->next != 0)
    {
        a_this->next->prev = a_this->prev;
    }
    if(a_this->prev != 0)
    {
        a_this->prev->next = a_this->next;
    }
    else
    {
        a_this->hash->tab[hindex(a_this->hkey,a_this->hash->size)] = a_this->next;
    }
    a_this->hash->nelem--;

    free(a_this);
}
