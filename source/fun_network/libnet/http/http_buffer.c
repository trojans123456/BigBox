#include <stdlib.h>

#include <string.h>

#include "http_buffer.h"

#define HTTP_BUFFER_DEFAULT_SIZE    64

struct http_buffer__
{
    size_t length; /*buffer所分配的大小*/
    size_t used; /*容器所用字节数*/
    char   *data;
};

http_buffer_t *http_buffer_new(size_t size)
{
    http_buffer_t *buf = NULL;

    if(0 == size)
        size = HTTP_BUFFER_DEFAULT_SIZE;

    buf = (http_buffer_t *)calloc(1,sizeof(http_buffer_t));
    if(!buf)
        return NULL;

    buf->data = (char *)calloc(1,size);
    if(!buf->data)
    {
        free(buf);
        return NULL;
    }

    buf->length = size;
    buf->data[0] = '\0';
    buf->used = 1; /*防止溢出*/

    return buf;
}

void http_buffer_delete(http_buffer_t *buf)
{
    if(buf)
    {
        if(0 != buf->length)
        {
            buf->length = 0;
            free(buf->data);

            buf->data = NULL;
            buf->used = 0;

            free(buf);
            buf = NULL;
        }
    }
}

char *http_get_data(http_buffer_t *buf)
{
    if(!buf)
        return NULL;
    return buf->data;
}

size_t http_get_used(http_buffer_t *buf)
{
    if(!buf)
        return 0;
    return buf->used;
}

size_t http_get_length(http_buffer_t *buf)
{
    if(!buf)
        return 0;
    return buf->length;
}

void http_buffer_grow(http_buffer_t *buf, size_t newsize)
{
    if(newsize > buf->length)
    {
        buf->length = ((newsize / HTTP_BUFFER_DEFAULT_SIZE) + 1) * HTTP_BUFFER_DEFAULT_SIZE;
        buf->data = realloc(buf->data,buf->length);
    }
}

static size_t http_va_args(va_list *ap)
{
    size_t total = 0;
    char *next = NULL;

    while((next = va_arg(*ap,char * )) != NULL)
    {
        total += strlen(next);
    }
    return total;
}

static size_t http_va_copy(char *str,va_list *ap)
{
    const char *next = NULL;
    size_t len = 0;
    while((next = va_arg(*ap,char * )) != NULL)
    {
        len = strlen(next);
        memcpy(str,next,len);
        str += len;
    }
}

void http_buffer_append(http_buffer_t *buf,...)
{
    va_list ap;
    size_t total = 0;

    va_start(ap,buf);
    total = buf->used + http_va_args(&ap);
    va_end(ap);

    http_buffer_grow(buf,total);

    va_start(ap,buf);
    http_va_copy(buf->data + buf->used - 1,&ap);
    va_end(ap);

    buf->used += total;
    buf->data[total - 1] = '\0';

}


static int http_vasprintf(char **ptr,const char *format,va_list ap)
{
    va_list ap_copy;

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

void http_buffer_sprintf(http_buffer_t *buf, char *fmt,...)
{
    va_list ap;
    int len = 0;
    char *res = NULL;

    va_start(ap,fmt);
    len = http_vasprintf(&res,fmt,ap);
    va_end(ap);

    if(len < 0)
    {
        if(res)
            free(res);
        return ;
    }

    http_buffer_cat(buf,res,len);
    free(res);
}

void http_buffer_cat(http_buffer_t *buf, const char *str, size_t len)
{
    http_buffer_grow(buf,buf->used + len);

    memcpy(buf->data + buf->used - 1,str,len);

    buf->used += len;
    buf->data[buf->used - 1] = '\0';
}

int http_isspace(int x)
{
    if(x==' '||x=='\t'||x=='\n'||x=='\f'||x=='\b'||x=='\r')
        return 1;
    else
        return 0;
}

int http_isdigit(int x)
{
    if(x<='9'&&x>='0')
        return 1;
    else
        return 0;
}

int http_tolower(int c)
{
    if ((c >= 'A') && (c <= 'Z'))
        return c + ('a' - 'A');
    return c;
}

int http_match(const char *str1,const char *str2)
{
    int c1, c2;
    do {
        c1 = *str1++;
        c2 = *str2++;
    } while((c1 == c2) && c1);

    return c1 - c2;
}

int http_nmatch(const char *str1,const char *str2,size_t n)
{
    int i = 0;
    int c1, c2;
    do {
        c1 = *str1++;
        c2 = *str2++;
        i++;
    } while(((c1 == c2) && c1) && (i < n));

    return c1 - c2;
}

/** 忽略大小写 匹配*/
int http_ignore_case_match(const char *str1,const char *str2)
{
    int c1, c2;

    do {
        c1 = http_tolower(*str1++);
        c2 = http_tolower(*str2++);
    } while((c1 == c2) && c1);

    return c1 - c2;
}

int http_ignore_case_nmatch(const char *str1,const char *str2,size_t n)
{
    int i = 0;
    int c1, c2;

    do {
        c1 = http_tolower(*str1++);
        c2 = http_tolower(*str2++);
        i++;
    } while(((c1 == c2) && c1) && (i < n));

    return c1 - c2;
}

char *http_right_strchr(char *s,char x)
{
    int i = strlen(s);
    if (!(*s))
        return 0;
    while (s[i-1]) {
        if (strchr(s + (i - 1), x)) {
            return (s + (i - 1));
        } else {
            i--;
        }
    }
    return 0;
}

/** 给str追加一个new_str */
char *http_assign_string(char **str,const char *new_str,int len)
{
    int l = len;
    char *old_str = *str;

    if (new_str == NULL) {
        return NULL;
    }

    if (l <= 0) {
        l = strlen(new_str);
    }

    if (old_str) {
        old_str = realloc(old_str, l + 1);
    } else {
        old_str = malloc(l + 1);
    }

    if (NULL != old_str) {
        memcpy(old_str, new_str, l);
        *str = old_str;
        old_str[l] = '\0';
    }
    return old_str;
}

void http_release_string(char *str)
{
    if (NULL != str)
        free(str);
}

int http_atoi(const char *nptr)
{
    int c;              /* current char */
    int total;          /* current total */
    int sign;           /* if '-', then negative, otherwise positive */

    /* skip whitespace */
    while (http_isspace((int)(unsigned char)*nptr))
        ++nptr;

    c = (int)(unsigned char)*nptr++;
    sign = c;                       /* save sign indication */

    if (c == '-' || c == '+')
        c = (int)(unsigned char)*nptr++;    /* skip sign */

    total = 0;

    while (http_isdigit(c)) {
        total = 10 * total + (c - '0');     /* accumulate digit */
        c = (int)(unsigned char)*nptr++;    /* get next char */
    }

    if (sign == '-')
        return -total;
    else
        return total;   /* return result, negated if necessary */
}

char *http_itoa(int value, char *string, int radix)
{
    char tmp[33];
    char* tp = tmp;
    int i;
    unsigned v;
    int sign;
    char* sp;

    if (radix > 36 || radix <= 1)
        return 0;

    sign = (radix == 10 && value < 0);

    if (sign)
        v = -value;

    else
        v = (unsigned)value;

    while (v || tp == tmp) {
        i = v % radix;
        v = v / radix;

        if (i < 10)
            *tp++ = i + '0';
        else
            *tp++ = i + 'a' - 10;
    }

    if (string == 0)
        string = (char*)malloc((tp - tmp) + sign + 1);

    sp = string;

    if (sign)
        *sp++ = '-';

    while (tp > tmp)
        *sp++ = *--tp;
    *sp = 0;

    return string;
}
