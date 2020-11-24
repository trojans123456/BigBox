#ifndef __STRINGBUF_H
#define __STRINGBUF_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct strbuf
{
    char *buf; /*分配的缓存*/
    int len; /* 已使用长度*/
    int size; /*当前分配的内存大小*/
    int capacity;/*最大限度 动态增长到该值报错*/
    int (*append)(struct strbuf *sb,const void *str,...);
    /*使用时直接用sb.buf 消费完后调用consume函数*/
    int (*consume)(struct strbuf *sb,int len);
    void (*print)(struct strbuf *sb);
}strbuf_t;

void strbuf_init(strbuf_t *sb,int capacity);
void strbuf_release(strbuf_t *sb);

#ifdef __cplusplus
}
#endif

#endif
