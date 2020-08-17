#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "data_buffer.h"

#ifndef jump_to_error_if
    #define jump_to_error_if(A) if(A) {goto error;}
#endif

#ifndef jump_to_error_unless
    #define jump_to_error_unless(A) if(A) {goto error;}
#endif

struct data_buffer
{
    char *contents;
    int bytes_used;
    int total_size;
};

Buffer *buffer_alloc(int initial_size)
{
    Buffer *buf = malloc(sizeof(Buffer));
    char   *tmp = calloc(1, initial_size * sizeof(char));

    jump_to_error_if(buf == NULL || tmp == NULL);

    buf->contents   = tmp;
    buf->bytes_used = 0;
    buf->total_size = initial_size;

    return buf;

error:
    if (buf) { buffer_free(buf); }
    if (tmp) { free(tmp); }

    return NULL;
}

int buffer_strlen(Buffer *buf)
{
    return buf->bytes_used;
}

void buffer_free(Buffer *buf)
{
    free(buf->contents);
    free(buf);
}

int buffer_has_space(Buffer *buf, int desired_length)
{
    int bytes_remaining = buf->total_size - buf->bytes_used;

    printf("Requesting %d bytes, %d available", desired_length, bytes_remaining);

    printf("-- %d\n",desired_length <= bytes_remaining);
    return (desired_length <= bytes_remaining) ? 0 : 1;
}

int buffer_grow(Buffer *buf, int minimum_size)
{
    int factor = buf->total_size;

    if (factor < minimum_size) {
        factor = minimum_size;
    }

    int new_size = factor * 2;

    printf("Growing buffer from %d to %d bytes", buf->total_size, new_size);

    char *tmp = realloc(buf->contents, new_size * sizeof(char));
    jump_to_error_if(tmp == NULL);

    buf->contents   = tmp;
    buf->total_size = new_size;

    return 0;

error:
    return -1;
}

static void buffer_cat(Buffer *buf, char *append, int length)
{
#if 0
    int i               = 0;
    int bytes_copied    = 0;
    int buffer_position = 0;

    for (i = 0; i < length; i++) {
        if (append[i] == '\0') { break; }

        buffer_position = buf->bytes_used + i;
        *(buf->contents + buffer_position) = append[i];

        bytes_copied++;
    }

    buf->bytes_used += bytes_copied;
    *(buf->contents + buf->bytes_used) = '\0';
#else
    int buffer_position = buf->bytes_used;
    memcpy(&buf->contents[buffer_position],append,length);
    buf->bytes_used += length;
#endif
}

int buffer_append(Buffer *buf, char *append, int length)
{
    int status         = 0;
    int desired_length = length + 1; // Space for NUL byte

    if (!buffer_has_space(buf, desired_length)) {
        status = buffer_grow(buf, desired_length);
        jump_to_error_unless(status == 0)
    }
    printf("-------cat\n");
    buffer_cat(buf, append, length);

    return 0;

error:
    return -1;
}

/** 自动分配字符串  */
static int vasprintf__ (char **ptr, const char *format, va_list ap)
{
    va_list ap_copy;

    /* Make sure it is determinate, despite manuals indicating otherwise */
    *ptr = 0;

    va_copy(ap_copy, ap);
    int count = vsnprintf(NULL, 0, format, ap);
    if (count >= 0)
    {
        char* buffer = malloc(count + 1);
        if (buffer != NULL)
        {
            count = vsnprintf(buffer, count + 1, format, ap_copy);
            if (count < 0)
                free(buffer);
            else
                *ptr = buffer;
        }
    }
    va_end(ap_copy);

    return count;

}

static int asprintf__ (char **ptr, const char *format, ...)
{
    va_list ap;
    int ret;

    *ptr = NULL;

    va_start (ap, format);
    ret = vasprintf__ (ptr, format, ap);
    va_end (ap);

    return ret;
}

int buffer_appendf(Buffer *buf, const char *format, ...)
{
    char *tmp = NULL;
    int bytes_written = -1, status = 0;

    va_list argp;
    va_start(argp, format);

    bytes_written = vasprintf__(&tmp, format, argp);

    jump_to_error_if(bytes_written < 0);

    va_end(argp);

    printf("--- %s %d \n",tmp,bytes_written);
    status = buffer_append(buf, tmp, bytes_written);
    jump_to_error_unless(status == 0);

    free(tmp);

    return 0;

error:
    if (tmp != NULL) { free(tmp); }
    return -1;
}

int buffer_nappendf(Buffer *buf, size_t length, const char *format, ...)
{
    int status        = 0,
        printf_length = length + 1;

    char *tmp  = calloc(1, printf_length * sizeof(char));

    jump_to_error_if(tmp == NULL);

    va_list argp;
    va_start(argp, format);

    status = vsnprintf(tmp, printf_length, format, argp);
    jump_to_error_if(status < 0);

    va_end(argp);

    status = buffer_append(buf, tmp, length);
    jump_to_error_unless(status == 0);

    free(tmp);

    return 0;

error:
    if (tmp != NULL) { free(tmp); }
    return -1;

}

char *buffer_to_string(Buffer *buf)
{
    char *result = calloc(1, buf->bytes_used + 1);
    strncpy(result, buf->contents, buffer_strlen(buf));

    return result;
}
