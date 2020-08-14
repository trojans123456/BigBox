#ifndef __BSEARCH_H
#define __BSEARCH_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 二分查找
 * @param 要查找元素的数组(整个元素首地址)
 * @param 要查找的元素
 * @param 元素个数
 * @param 每个元素的大小
 * @return
 */
void *bsearch(const void *base,const void *key,size_t num,size_t typesize,
              int (*cmp)(const void *key,const void *elt));

#ifdef __cplusplus
}
#endif

#endif
