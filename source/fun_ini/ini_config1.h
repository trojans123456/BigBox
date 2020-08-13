#ifndef __CONFIG_INI_1_H
#define __CONFIG_INI_1_H

#include "string_dic_p.h"

#ifdef __cplusplus
extern "C" {
#endif

struct config_ini__
{
    string_dic_t *d;
};
typedef struct config_ini__ config_ini_t;

/**
 * @brief 加载一个ini文件并解析
 * @param a_name
 * @return
 */
config_ini_t *config_init_load(const char *a_name);

/**
 * @brief 释放加载的ini对象
 * @param a_ini
 */
void config_ini_free(config_ini_t *a_ini);

/**
 * @brief 初始化一个对象
 * @param a_ini
 */
config_ini_t *config_ini_new();

/**
 * @brief 保存到文件
 * @param a_name
 */
int config_ini_save(config_ini_t *a_ini,const char *a_name);

/**
 * @brief 获取文件中的section节数
 * @param a_ini
 * @return
 */
int config_ini_get_section_num(config_ini_t *a_ini);

/**
 * @brief 获取第n节的名称
 * @param a_ini
 * @return
 */
char *config_ini_get_section_name(config_ini_t *a_ini,int n);

/**
 * @brief 将字典内容保存到文件
 * @param a_ini
 * @param f
 */
void config_ini_save_ini(config_ini_t *a_ini,FILE *f);

/**
 * @brief 将一个section节保存到文件中
 * @param a_ini
 * @param sec
 * @param f
 */
void config_ini_savesection_to_file(config_ini_t *a_ini,char *sec,FILE *f);

/**
 * @brief 打印到f
 * @param a_ini
 * @param f
 */
void config_ini_dump(config_ini_t *a_ini,FILE *f);

/**
 * @brief 获取一个节中key的个数
 * @param a_ini
 * @param section
 * @return
 */
int config_ini_get_section_keys_num(config_ini_t *a_ini,const char *section);

/**
 * @brief 获取一个节中key列表
 * @param a_ini
 * @param section
 * @return 返回key列表的首地址
 */
char **config_ini_get_section_keys(config_ini_t *a_ini,const char *section);

/**
 * @brief 获取对应key的value值，字符串
 * @param a_ini
 * @param key
 * @param def
 * @return
 *
 * Action!!
 * key的值需添加section  section:key
 */
char *config_ini_getString(config_ini_t *a_ini,const char *key,char *def);

/**
 * @brief 获取value值 int类型
 * @param a_ini
 * @param key
 * @param notfound
 * @return
 *
 * key = section:key
 */
int config_ini_getInt(config_ini_t *a_ini,const char *key,int notfound);

/**
 * @brief 获取value值 double类型
 * @param a_ini
 * @param key
 * @param notfound
 * @return
 */
double config_ini_getDouble(config_ini_t *a_ini,const char *key,double notfound);

/**
 * @brief 获取bool
 * @param a_ini
 * @param key
 * @param notfound
 * @return
 */
int config_ini_getBoolean(config_ini_t *a_ini,const char *key,int notfound);

/**
 * @brief 设置key=value
 * @param a_ini
 * @param entry
 * @param val
 * @return
 */
int config_ini_set(config_ini_t *a_ini,const char *entry,const char *val);

void config_ini_unset(config_ini_t *a_ini,const char *entry);

int config_ini_find_entry(config_ini_t *a_ini,const char *entry);

#ifdef __cplusplus
}
#endif

#endif
