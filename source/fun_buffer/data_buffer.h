#ifndef __DATA_BUFFER_H
#define __DATA_BUFFER_H


#ifdef __cplusplus
extern "C" {
#endif



typedef struct data_buffer Buffer;

Buffer *buffer_alloc(int initial_size);

int buffer_strlen(Buffer *buf);

void buffer_free(Buffer *buf);

int buffer_append(Buffer *buf,char *append,int length);

int buffer_appendf(Buffer *buf,const char *format,...);

int buffer_nappendf(Buffer *buf,size_t length,const char *format,...);

char *buffer_to_string(Buffer *buf);


#ifdef __cplusplus
}
#endif

#endif

