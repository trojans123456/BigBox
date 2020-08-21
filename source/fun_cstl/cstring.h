#ifndef __CSTRING_H
#define __CSTRING_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cstring__ cstring;

/**
 * @brief 分配一个新cstring
 * @return
 */
cstring *cstr_new();

/**
 * @brief 分配一个n字节的cstring
 * @param n
 * @return
 */
cstring *cstr_new_with_size(size_t n);

cstring *cstr_new_with_string(char *str);

cstring *cstr_new_with_string_length(char *str,size_t n);

cstring *cstr_new_with_copy(char *str);

/**
 * @brief 返回buffer长度(分配的最大长度)
 * @param self
 * @return
 */
size_t cstr_size(cstring *self);

/**
 * @brief 返回字符串长度
 * @param self
 * @return
 */
size_t cstr_length(cstring *self);

/**
 * @brief 释放cstring
 * @param self
 */
void cstr_free(cstring *self);

/**
 * @brief 将str添加到self内容的最前面
 * @param self
 * @param str
 * @return
 */
int cstr_prepend(cstring *self,char *str);

int cstr_append(cstring *self,const char *str);

int cstr_appendf(cstring *self, const char *format, ...);

int cstr_append_n(cstring *self, const char *str, size_t len);

int cstr_equals(cstring *self, cstring *other);

ssize_t cstr_indexof(cstring *self, char *str);

/**
 * @brief 将from...to的数据拷贝到self
 * @param self
 * @param from
 * @param to
 * @return
 */
cstring * cstr_slice(cstring *self, size_t from, ssize_t to);

/**
 * @brief 释放多余内存
 * @param self
 * @return
 */
ssize_t cstr_compact(cstring *self);

void cstr_fill(cstring *self, int c);

void cstr_clear(cstring *self);

/**
 * @brief 去掉开头的空格
 * @param self
 */
void cstr_trim_left(cstring *self);

/**
 * @brief 去掉末尾的空格
 * @param self
 */
void cstr_trim_right(cstring *self);

/**
 * @brief 去掉前后的空格
 * @param self
 */
void cstr_trim(cstring *self);

void cstr_print(cstring *self);

//#define cstr_string(self) (self->data)
char *cstr_string(cstring *self);

#ifdef __cplusplus
}
#endif

#endif
