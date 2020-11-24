#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "stringbuf.h"

#define MIN_SIZE (4 << 10)
static int calc_size(int real_size,int max_size)
{
    int size;
    for(size = MIN_SIZE;size < max_size;size *= 2)
    {
        if(size >= real_size)
            return size;
    }
    return max_size;
}

static int append__(strbuf_t *sb,const void *str,...)
{
    if(!sb)
        return -1;
    va_list ap;
    va_start(ap,str);
    char empty = '\0';
    int strlen = vsnprintf(&empty,0,str,ap) + 1; /* +1 的话多个\0*/
    va_end(ap);

    int need_size = sb->len + strlen;
    /* 超过最大限度*/
    if(need_size > sb->capacity)
        return -1;

    /* 超过当前缓冲size 需要加大size*/
    if(need_size > sb->size)
    {
        /* 计算新开辟空间大小*/
        int size = calc_size(need_size,sb->capacity);
        if(sb->buf)
        {
            sb->buf = (char*)realloc(sb->buf,size);
        }
        else
        {
            sb->buf = (char*)malloc(size);
        }

        /* 分配失败*/
        if(!sb->buf)
        {
            return -2;
        }

        sb->size = size;
    }

    /*拷贝数据*/
    va_start(ap,str);
    vsnprintf(sb->buf + sb->len,strlen,str,ap);
    va_end(ap);

    sb->len += strlen;

    return 0;
}

static int consume__(strbuf_t *sb,int len)
{
    if(!sb || len < 0)
        return -1;
    if(sb->len > len)
    {
        memmove(sb->buf,sb->buf + len,sb->len - len);
        sb->len -= len;
    }
    else
    {
        sb->len = 0;
    }
    return 0;
}

static void print__(strbuf_t *sb)
{
    if(sb)
    {
        printf("string_buffer[%p]: len[%d] size[%d] cap[%d]",
            sb->buf, sb->len, sb->size, sb->capacity);
    }
}

void strbuf_init(strbuf_t *sb, int capacity)
{
    if(sb)
    {
        sb->buf = NULL;
        sb->len = 0;
        sb->size = 0;
        sb->capacity = capacity;
        sb->append = append__;
        sb->consume = consume__;
        sb->print = print__;
    }
}

void strbuf_release(strbuf_t *sb)
{
    if(sb)
    {
        if(sb->buf)
            free(sb->buf);
        strbuf_init(sb,0);
    }
}
