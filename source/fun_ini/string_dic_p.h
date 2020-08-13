#ifndef __STRING_DIC_P_H
#define __STRING_DIC_P_H

#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct string_dic__
{
    int             n ;     /** Number of entries in dictionary */
    int             size ;  /** Storage size */
    char        **  val ;   /** List of string values */
    char        **  key ;   /** List of string keys */
    unsigned     *  hash ;  /** List of hash values for keys */
};
typedef struct string_dic__ string_dic_t;

int string_dic_get_entries(string_dic_t *a_this);

/**
 * @brief 创建一个字典对象
 * @param size  字典初始值大小 （字典条数）
 * @return
 *
 * 分配一个string-key:value对。内部用hash实现
 */
string_dic_t *string_dic_new(size_t size);

/**
 * @brief 删除一个字典对象
 * @param a_this
 */
void string_dic_del(string_dic_t *a_this);

/**
 * @brief 从字典里根据key获取value值
 * @param a_this
 * @param key
 * @param def   默认值，如果没有找到键，返回的默认值
 * @return
 */
char *string_dic_get(string_dic_t *a_this,const char *key,char *def);

/**
 * @brief string_dic_set
 * @param a_this
 * @param key  内部会自动分配空间
 * @param value 内部会自动分空间
 * @return  0-success
 *
 * Action!!
 * key值不能为空，value可为空；如果key存在则相当于修改key的值，
 * 不存在则创建该key
 */
int string_dic_set(string_dic_t *a_this,const char *key,const char *value);

/**
 * @brief 从字典中删除一个key
 * @param a_this
 * @param key
 */
void string_dic_unset(string_dic_t *a_this,const char *key);

/**
 * @brief 将字典以key=value形式转储到文件中
 * @param a_this
 * @param out
 */
void string_dic_dump(string_dic_t *a_this,FILE *out);

#ifdef __cplusplus
}
#endif

#endif
