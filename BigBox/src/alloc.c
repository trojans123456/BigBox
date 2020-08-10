#include <stdlib.h>

#include "alloc.h"

#define foreach_arg(_arg, _addr, _len, _first_addr, _first_len) \
    for (_addr = (_first_addr), _len = (_first_len); \
        _addr; \
        _addr = va_arg(_arg, void **), _len = _addr ? va_arg(_arg, size_t) : 0)

#define C_PTR_ALIGN	(sizeof(size_t))
#define C_PTR_MASK	(-C_PTR_ALIGN)

void *__calloc_a(size_t len,...)
{
    va_list ap, ap1;
        void *ret;
        void **cur_addr;
        size_t cur_len;
        int alloc_len = 0;
        char *ptr;

        va_start(ap, len);

        va_copy(ap1, ap);
        foreach_arg(ap1, cur_addr, cur_len, &ret, len)
            alloc_len += (cur_len + C_PTR_ALIGN - 1 ) & C_PTR_MASK;
        va_end(ap1);

        ptr = calloc(1, alloc_len);
        if (!ptr) {
            va_end(ap);
            return NULL;
        }

        alloc_len = 0;
        foreach_arg(ap, cur_addr, cur_len, &ret, len) {
            *cur_addr = &ptr[alloc_len];
            alloc_len += (cur_len + C_PTR_ALIGN - 1) & C_PTR_MASK;
        }
        va_end(ap);

        return ret;
}
