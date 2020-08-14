#include <stdio.h>

#include "bsearch.h"

void *bsearch(const void *base, const void *key, size_t num, size_t typesize, int (*cmp)(const void *, const void *))
{
    size_t start = 0, end = num;
    int result;

    while (start < end) {
        size_t mid = start + (end - start) / 2;

        result = cmp(key, base + mid * typesize);
        if (result < 0)
            end = mid;
        else if (result > 0)
            start = mid + 1;
        else
            return (void *)base + mid * typesize;
    }

    return NULL;
}
