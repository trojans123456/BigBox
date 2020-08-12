#ifndef __ALLOC_H
#define __ALLOC_H

#include <stdarg.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define calloc_a(len,...)   __calloc_a(len,##__VA_ARGS__,NULL)

void *__calloc_a(size_t len,...);


#ifdef __cplusplus
}
#endif

#endif
