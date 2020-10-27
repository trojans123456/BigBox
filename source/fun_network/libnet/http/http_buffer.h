#ifndef __HTTP_BUFFER_H
#define __HTTP_BUFFER_H

#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct http_buffer__ http_buffer_t;

/**
 * @brief http_buffer_new 创建一个buffer
 * @param size
 * @return
 */
http_buffer_t *http_buffer_new(size_t size);

void http_buffer_delete(http_buffer_t *buf);

char *http_get_data(http_buffer_t *buf);
size_t http_get_used(http_buffer_t *buf);
size_t http_get_length(http_buffer_t *buf);

/**
 * @brief http_buffer_append
 * @param buf
 * @param fmt  必须以NULL结尾
 *
 * eg:
 *  http_buffer_append(buf,"xx","test",NULL);
 */
#define buffer_append(buf,...)     http_buffer_append(buf,__VA_ARGS__,NULL)
void http_buffer_append(http_buffer_t *buf,...);

void http_buffer_sprintf(http_buffer_t *buf,char *fmt,...);

void http_buffer_cat(http_buffer_t *buf,const char *str,size_t len);


/********************* utils *********************/
int http_isspace(int x);
int http_isdigit(int x);
int http_tolower(int c);
int http_match(const char *s1,const char *s2);
int http_nmatch(const char *s1,const char *s2,size_t n);
/** 忽略大小写 匹配*/
int http_ignore_case_match(const char *s1,const char *s2);
int http_ignore_case_nmatch(const char *s1, const char *s2, size_t n);

char *http_right_strchr(char *s,char x);
/** 给str追加一个new_str */
char *http_assign_string(char **str,const char *new_str,int len);

void http_release_string(char *str);

int http_atoi(const char *nptr);
char *http_itoa(int value,char *string,int radix);

#ifdef __cplusplus
}
#endif

#endif
