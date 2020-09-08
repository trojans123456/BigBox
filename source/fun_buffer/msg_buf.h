#ifndef __MSG_BUF_H
#define __MSG_BUF_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


struct msg_buf
{
    size_t length;
    size_t used;
    char *data;
};
typedef struct msg_buf msg_buf_t;

msg_buf_t msg_buf_init(size_t size);


void msg_buf_grow(msg_buf_t *buf,size_t newsize);

void msg_buf_release(msg_buf_t *buf);

void msg_buf_concat(msg_buf_t *buf,...);

void msg_buf_cover(msg_buf_t *buf,...);

void msg_buf_append(msg_buf_t *buf,const char *str,size_t len);

void msg_buf_pointer(msg_buf_t *buf,const char *str,size_t len);

char *msg_buf_get_data(msg_buf_t *buf);

size_t msg_buf_get_used(msg_buf_t *buf);

size_t msg_buf_get_length(msg_buf_t *buf);

#ifdef __cplusplus
}
#endif

#endif
