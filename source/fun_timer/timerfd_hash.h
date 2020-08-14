#ifndef __TIMERFD_HASH_H_
#define __TIMERFD_HASH_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif



#define SD_HASH_FULLTAB	2	/* rehash when table gets this x full */
#define SD_HASH_GROWTAB 4	/* grow table by this factor */
#define SD_HASH_DEFAULT_SIZE 10 /* self explenatory */

#define hindex(h,n)     ((h) % (n))  //除法散列法

typedef struct hash_ops
{
    unsigned int (*hash)(const void *);
    int (*compare)(const void *,const void *);
    void* (*key_dup)(const void *);
    void (*key_free)(void *);   //key
    void* (*data_dup)(const void *); //value
    void (*data_free)(void *);
}hash_ops_t;

//迭代器
typedef struct hash_iter
{
    void *key;
    void *data;
    struct hash *hash;
    unsigned int hkey;
    struct hash_iter *next;
    struct hash_iter *prev;
    int foreach;
}hash_iter_t;

typedef struct hash
{
    size_t      nelem;
    size_t      size;
    hash_iter_t **tab;
    const hash_ops_t *ops;
}hash_t;

//创建一个hash  a_size 是桶大小容纳多少个key
extern hash_t *hash_new(size_t a_size,const hash_ops_t *a_ops);
extern void hash_delete(hash_t *a_this);
extern void hash_clear(hash_t *a_this);
extern hash_iter_t *hash_lookup(hash_t *a_this,const void *a_key);
//查找一个key，如果不存在就添加它
extern hash_iter_t *hash_lookadd(hash_t *a_this,const void *a_key);
//添加一个key:value 如果key存在先删除再创建
extern hash_iter_t *hash_add(hash_t *a_this,const void *a_key,const void *a_data);
extern void hash_del(hash_t *a_this,const void *a_key);

typedef unsigned int (*hash_func_t)(void *a_key,void *a_data,void *a_userdata);
extern void hash_foreach(hash_t *a_this,hash_func_t a_func,void *a_userdata);

extern unsigned int hash_get_nelem(hash_t *a_this);
//获取数据个数
extern unsigned int hash_get_size(hash_t *a_this);

//指向第一个数据
extern hash_iter_t *hash_begin(hash_t *a_this);
extern hash_iter_t *hash_end(hash_t *a_this);
//迭代器遍历
extern hash_iter_t *hash_iter_next(hash_iter_t *a_this);
extern hash_iter_t *hash_iter_prev(hash_iter_t *a_this);
extern void hash_iter_del(hash_iter_t *a_this);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif
