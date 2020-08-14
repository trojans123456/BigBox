#ifndef ___ALLOC_P_H
#define ___ALLOC_P_H

#include <stdlib.h>
#include <string.h>

#include <assert.h>

#if defined(__cplusplus)
extern "C" {
#endif


static inline void *Malloc(size_t size)
{
    void *ptr = malloc(size);
    assert(ptr);
    return ptr;
}

static inline void *Calloc(size_t nmemb,size_t size)
{
    void *ptr = calloc(nmemb,size);
    assert(ptr);
    return ptr;
}

static inline void *Realloc(void *ptr,size_t size)
{
    void *ret = realloc(ptr,size);
    assert(ret);
    return ret;
}

static inline void *Free(void *ptr)
{
    if(ptr)
        free(ptr);
}

static inline char *Strdup(const char *str)
{
    char *ptr = strdup(str);
    assert(ptr);
    return ptr;
}

#if defined(__cplusplus)
}
#endif

#endif // ___ALLOC_P_H
