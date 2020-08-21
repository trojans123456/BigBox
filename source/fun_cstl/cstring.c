#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "cstring.h"

struct cstring__
{
    size_t len;
    char *alloc; /*指向一个缓存*/
    char *data; /*指向数据*/
};

/** 默认大小 */
#ifndef BUFFER_DEFAULT_SIZE
#define BUFFER_DEFAULT_SIZE 64
#endif

/*
 * Compute the nearest multiple of `a` from `b`.
 */

#define nearest_multiple_of(a, b) \
  (((b) + ((a) - 1)) & ~((a) - 1))


cstring *cstr_new()
{
    return cstr_new_with_size(BUFFER_DEFAULT_SIZE);
}

cstring *cstr_new_with_size(size_t n)
{
    cstring *self = calloc(1,sizeof(cstring));
    if(!self)
    {
        return NULL;
    }

    self->len = n;
    self->data = self->alloc = calloc(n + 1,1);

    return self;
}

cstring *cstr_new_with_string(char *str)
{
    return cstr_new_with_string_length(str,strlen(str));
}

cstring *cstr_new_with_string_length(char *str,size_t n)
{
    cstring *self = calloc(1,sizeof(cstring));
    if(!self)
    {
        return NULL;
    }

    self->len = n;
    self->data = self->alloc = str;

    return self;
}

cstring *cstr_new_with_copy(char *str)
{
    size_t len = strlen(str);
    cstring *self = cstr_new_with_size(len);
    if(!self)
    {
        return NULL;
    }
    memcpy(self->alloc,str,len);
    self->data = self->alloc;

    return self;
}

size_t cstr_size(cstring *self)
{
    return self->len;
}

size_t cstr_length(cstring *self)
{
    return strlen(self->data);
}

void cstr_free(cstring *self)
{
    free(self->alloc);
    free(self);
}

int cstr_resize(cstring *self,size_t n)
{
    n = nearest_multiple_of(1024,n);
    self->len = n;
    self->alloc = self->data = realloc(self->alloc,n + 1);
    if(!self->alloc)
    {
        return -1;
    }
    self->alloc[n] = '\0';

    return 0;
}

int cstr_prepend(cstring *self,char *str)
{
    size_t len = strlen(str);
    size_t prev = strlen(self->data);
    size_t needed = len + prev;

    // enough space
    if (self->len > needed) goto move;

    // resize
    int ret = cstr_resize(self, needed);
    if (-1 == ret) return -1;

    // move
move:
    memmove(self->data + len, self->data, len + 1);
    memcpy(self->data, str, len);

    return 0;
}

int cstr_append(cstring *self,const char *str)
{
  return cstr_append_n(self, str, strlen(str));
}

int cstr_appendf(cstring *self, const char *format, ...)
{
    va_list ap;
    va_list tmpa;
    char *dst = NULL;
    int length = 0;
    int required = 0;
    int bytes = 0;

    va_start(ap, format);

    length = cstr_length(self);

    // First, we compute how many bytes are needed
    // for the formatted string and allocate that
    // much more space in the buffer.
    va_copy(tmpa, ap);
    required = vsnprintf(NULL, 0, format, tmpa);
    va_end(tmpa);
    if (-1 == cstr_resize(self, length + required)) {
        va_end(ap);
        return -1;
    }

    // Next format the string into the space that we
    // have made room for.
    dst = self->data + length;
    bytes = vsnprintf(dst, 1 + required, format, ap);
    va_end(ap);

    return bytes < 0
            ? -1
            : 0;
}

int cstr_append_n(cstring *self, const char *str, size_t len)
{
    size_t prev = strlen(self->data);
    size_t needed = len + prev;

    // enough space
    if (self->len > needed) {
        strncat(self->data, str, len);
        return 0;
    }

    // resize
    int ret = cstr_resize(self, needed);
    if (-1 == ret) return -1;
    strncat(self->data, str, len);

    return 0;
}

int cstr_equals(cstring *self, cstring *other)
{
    return 0 == strcmp(self->data, other->data);
}

ssize_t cstr_indexof(cstring *self, char *str)
{
    char *sub = strstr(self->data, str);
    if (!sub) return -1;
    return sub - self->data;
}

cstring * cstr_slice(cstring *buf, size_t from, ssize_t to)
{
    size_t len = strlen(buf->data);

    // bad range
    if (to < from) return NULL;

    // relative to end
    if (to < 0) to = len - ~to;

    // cap end
    if (to > len) to = len;

    size_t n = to - from;
    cstring *self = cstr_new_with_size(n);
    memcpy(self->data, buf->data + from, n);
    return self;
}

ssize_t cstr_compact(cstring *self)
{
    size_t len = cstr_length(self);
    size_t rem = self->len - len;
    char *buf = calloc(len + 1,1);
    if(!buf)
    {
        return -1;
    }

    memcpy(buf,self->data,len);
    free(self->alloc);
    self->len = len;
    self->data = self->alloc = buf;

    return rem;
}

void cstr_fill(cstring *self, int c)
{
    memset(self->data,c,self->len);
}

void cstr_clear(cstring *self)
{
    cstr_fill(self,0);
}

void cstr_trim_left(cstring *self)
{
    int c;
    while ((c = *self->data) && isspace(c)) {
        ++self->data;
    }
}

void cstr_trim_right(cstring *self)
{
    int c;
    size_t i = cstr_length(self) - 1;
    while ((c = self->data[i]) && isspace(c)) {
        self->data[i--] = 0;
    }
}

void cstr_trim(cstring *self)
{
    cstr_trim_left(self);
    cstr_trim_right(self);
}

void cstr_print(cstring *self)
{
    int i;
    size_t len = self->len;

    printf("\n ");

    // hex
    for (i = 0; i < len; ++i) {
        printf(" %02x", self->alloc[i]);
        if ((i + 1) % 8 == 0) printf("\n ");
    }

    printf("\n");
}

char *cstr_string(cstring *self)
{
    return self->data;
}
