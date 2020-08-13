#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef __WIN32__
    #include <unistd.h>
#endif

#include "string_dic_p.h"
#if 0
struct string_dic__
{
    int             n ;     /** Number of entries in dictionary */
    int             size ;  /** Storage size */
    char        **  val ;   /** List of string values */
    char        **  key ;   /** List of string keys */
    unsigned     *  hash ;  /** List of hash values for keys */
};
#endif
/**
 * @brief 复制一个字符串
 * @param s
 * @return
 */
static char * xstrdup(const char * s)
{
    char *t = NULL;
    size_t len = 0;
    if(!s)
        return NULL;

    len = strlen(s) + 1;
    t = (char *)calloc(1,len);
    if(t)
    {
        memcpy(t,s,len);
    }
    return t;
}

/**
 * @brief 以2*size的形式分配空间，并将ptr拷贝进去类似realloc
 * @param ptr
 * @param size
 * @return
 */
static void *mem_double_alloc(void *ptr,size_t size)
{
    void *newptr = NULL;
    newptr = calloc(2 * size,1);
    if(newptr == NULL)
    {
        return NULL;
    }

    memcpy(newptr,ptr,size);
    free(ptr);

    return newptr;
}

/**
 * @brief 计算字符串key的散列值(hash)
 * @param key
 */
static unsigned string_dic_hash(const char *key)
{
    size_t      len ;
    unsigned    hash ;
    size_t      i ;

    len = strlen(key);
    for (hash=0, i=0 ; i<len ; i++) {
        hash += (unsigned)key[i] ;
        hash += (hash<<10);
        hash ^= (hash>>6) ;
    }
    hash += (hash <<3);
    hash ^= (hash >>11);
    hash += (hash <<15);
    return hash ;
}

/** Minimal allocated number of entries in a dictionary */
#define STR_DICT_MIN_SIZE   128

/**
 * @brief 创建一个字典对象
 * @param size  字典初始值大小 （字典条数）
 * @return
 *
 * 分配一个string-key:value对。内部用hash实现
 */
string_dic_t *string_dic_new(size_t size)
{
    string_dic_t *d = NULL;

    if(size < STR_DICT_MIN_SIZE)
        size = STR_DICT_MIN_SIZE;

    d = (string_dic_t *)calloc(1,sizeof(*d));
    if(d)
    {
        d->size = size;
        d->val = (char **)calloc(size,sizeof(*d->val));
        d->key = (char **)calloc(size,sizeof(*d->key));
        d->hash = (unsigned int *)calloc(size,sizeof(*d->hash));
    }

    return d;
}

/**
 * @brief 删除一个字典对象
 * @param a_this
 */
void string_dic_del(string_dic_t *d)
{
    int i;
    if(d == NULL)
        return ;

    for(i = 0;i < d->size;i++)
    {
        if(d->key[i] != NULL)
        {
            free(d->key[i]);
        }
        if(d->val[i] != NULL)
        {
            free(d->val[i]);
        }
    }

    free(d->val);
    free(d->key);
    free(d->hash);
    free(d);
}

/**
 * @brief 从字典里根据key获取value值
 * @param a_this
 * @param key
 * @param def   默认值，如果没有找到键，返回的默认值
 * @return
 */
char *string_dic_get(string_dic_t *d,const char *key,char *def)
{
    unsigned int hash;
    int i;

    hash = string_dic_hash(key);

    for(i = 0;i < d->size;i++)
    {
        if(d->key[i] == NULL)
            continue;
        /*compare hash */
        if(hash == d->hash[i])
        {
            /* compare string */
            if(!strcmp(key,d->key[i]))
            {
                return d->val[i];
            }
        }
    }

    return def;
}

/**
 * @brief string_dic_set
 * @param a_this
 * @param key  该key存在则修改value值，否则新建key
 * @param value
 * @return  0-success
 *
 * Action!!
 * key值不能为空，value可为空；如果key存在则相当于修改key的值，
 * 不存在则创建该key
 */
int string_dic_set(string_dic_t *d,const char *key,const char *value)
{
    int i;
    unsigned int hash;

    if(d == NULL || key == NULL)
        return -1;

    hash = string_dic_hash(key);

    /* if value is already in str_dic*/
    if(d->n > 0)
    {
        for(i = 0; i < d->size; i++)
        {
            if(d->key[i] == NULL)
                continue;

            if(hash == d->hash[i])
            {
                /* same key*/
                if(!strcmp(key,d->key[i]))
                {
                    if(d->val[i] != NULL)
                    {
                        free(d->val[i]);
                    }
                    d->val[i] = value ? xstrdup(value) : NULL;

                    return 0;
                }
            }
        }
    }

    /* maybe needs to grow */
    if(d->n == d->size)
    {
        d->val = (char **)mem_double_alloc(d->val,d->size * sizeof(*d->val));
        d->key = (char **)mem_double_alloc(d->key,d->size * sizeof(*d->key));
        d->hash = (unsigned int *)mem_double_alloc(d->hash,d->size * sizeof(*d->hash));

        if((d->val == NULL) || (d->key == NULL) || (d->hash == NULL))
        {
            return -1;
        }

        d->size *= 2;
    }

    /* insert key:value */
    for(i = d->n;d->key[i];)
    {
        if(++i == d->size) i = 0;
    }

    /* copy key */
    d->key[i] = xstrdup(key);
    d->val[i] = value ? xstrdup(value) : NULL;
    d->hash[i] = hash;
    d->n++;

    return 0;
}

/**
 * @brief 从字典中删除一个key
 * @param a_this
 * @param key
 */
void string_dic_unset(string_dic_t *d,const char *key)
{
    unsigned hash;
    int i;
    if(key == NULL)
        return ;

    hash = string_dic_hash(key);
    for(i = 0;i < d->size;i++)
    {
        if(d->key[i] == NULL)
            continue;
        if(hash == d->hash[i])
        {
            if(!strcmp(key,d->key[i]))
            {
                /* found key */
                break;
            }
        }
    }

    if(i >= d->size)
    {
        /* key not found */
        return ;
    }

    free(d->key[i]);
    d->key[i] = NULL;
    if(d->val[i] != NULL)
    {
        free(d->val[i]);
        d->val[i] = NULL;
    }
    d->hash[i] = 0;
    d->n--;
}

/**
 * @brief 将字典以key=value形式转储到文件中
 * @param a_this
 * @param out
 */
void string_dic_dump(string_dic_t *d,FILE *out)
{
    int i;
    if(d == NULL || out == NULL)
        return ;

    if(d->n < 1)
    {
        fprintf(out,"empty string dictionary!\n");
        return ;
    }

    for(i = 0;i < d->size;i++)
    {
        if(d->key[i])
        {
            fprintf(out,"%20s\t[%s]\n",
                    d->key[i],d->val[i] ? d->val[i] : "undef");
        }
    }
}

int string_dic_get_entries(string_dic_t *a_this)
{
    if(a_this == NULL)
        return -1;

    return a_this->n;
}
