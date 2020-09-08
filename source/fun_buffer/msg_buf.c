
#include "msg_buf.h"

#define MSG_BUF_DEFAULT     64

msg_buf_t msg_buf_init(size_t size)
{
    msg_buf_t *buf;
    if(0 == size)
        size = MSG_BUF_DEFAULT;

    buf = (msg_buf_t *)calloc(1,sizeof(msg_buf_t));
    if(!buf)
        return NULL;

    buf->data = (char *)calloc(1,size);
    if(!buf->data)
    {
        free(buf);
        return NULL;
    }

    buf->length = size;
    buf->used = 1;

    return buf;
}


void msg_buf_grow(msg_buf_t *buf,size_t newsize)
{
    if(buf)
    {
        if(newsize > buf->length)
        {
            buf->length = ((newsize / MSG_BUF_DEFAULT) + 1) * MSG_BUF_DEFAULT;

            buf->data = (char *)realloc(buf->data,buf->length);
        }
    }
}

void msg_buf_release(msg_buf_t *buf)
{
    if(buf)
    {
        if(0 != buf->length)
        {
            buf->length = 0;
            free(buf->data);
        }

        buf->data = NULL;
        buf->used = 0;

        free(buf);
        buf = NULL;
    }
}

static void utils_concat(char *str,va_list *ap)
{
    char *next;
    size_t len;
    while((next = va_arg(*ap,char *)) != NULL)
    {
        len = strlen(next);
        memcpy(str,next,len);
        str += len;
    }
}

void msg_buf_concat(msg_buf_t *buf,...)
{
    va_list ap;
    size_t total;

    va_start(ap,buf);
    total = buf->used + utils_concat(&ap);
    va_end(ap);

    /* grow the buffer */
    msg_buf_grow(buf,total);

    va_start(ap,buf);
    utils_concat(buf->data + buf->used - 1,&ap);
    va_end(ap);

    buf->used = total;
    buf->data[total - 1] = '\0';
}

static size_t utils_count_conat(va_list *ap)
{
    size_t total = 0;
    char *next;

    while((next = va_arg(*ap,char *)) != NULL)
    {
        total += strlen(next);
    }
    return total;
}

void msg_buf_cover(msg_buf_t *buf,...)
{
    va_list ap;
    size_t total;

    va_start(ap,buf);
    total = 1 + utils_count_conat(&ap);
    va_end(ap);

    msg_buf_grow(buf,total);

    va_start(ap,buf);
    utils_concat(buf->data,&ap);
    va_end(ap);

    buf->used = total;
    buf->data[total - 1] = '\0';
}

void msg_buf_append(msg_buf_t *buf,const char *str,size_t len)
{
    msg_buf_grow(buf,buf->used + len);
    memcpy(buf->data + buf->used - 1,str,len);
    buf->used += len;
    buf->data[buf->used - 1] = '\0';
}

void msg_buf_pointer(msg_buf_t *buf,const char *str,size_t len)
{
    free(buf->data);
    buf->length = 0;

    buf->data = (char *)str;
    buf->used += len;


}

char *msg_buf_get_data(msg_buf_t *buf)
{
    return buf->data;
}

size_t msg_buf_get_used(msg_buf_t *buf)
{
    return buf->used;
}

size_t msg_buf_get_length(msg_buf_t *buf)
{
    return buf->length;
}
