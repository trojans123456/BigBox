#ifndef __QSORT_H
#define __QSORT_H

#ifdef __cplusplus
extern "C" {
#endif

void qsort(void* base,int nmemb,int size,int (*compar)(const void*,const void*));

#ifdef __cplusplus
}
#endif


#endif
