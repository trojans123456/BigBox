
#ifndef __DARRAY_H
#define __DARRAY_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

struct darray {
    void *array;
    size_t num;
    size_t capacity;
};

void darray_init(struct darray *dst);
void darray_free(struct darray *dst);
size_t darray_push_back(const size_t size, struct darray *dst, const void *item);
void darray_pop_back(const size_t size, struct darray *dst);
size_t darray_find(const size_t size, const struct darray *da, const void *item,
                const size_t idx);
void *darray_end(const size_t size, const struct darray *da);
void darray_reserve(const size_t size, struct darray *dst, const size_t capacity);
size_t darray_push_back_array(const size_t size, struct darray *dst,
                const void *array, const size_t num);
void darray_insert(const size_t element_size, struct darray *dst,
                const size_t idx, const void *item);
void darray_erase(const size_t element_size, struct darray *dst,
                const size_t idx);
void darray_erase_item(const size_t element_size,
                struct darray *dst, const void *item);
void darray_resize(const size_t element_size, struct darray *dst,
                const size_t size);


/*
 * belows are the APIs
 */

#define DARRAY(type)                    \
    union {                             \
        struct darray da;               \
        struct {                        \
            type *array;                \
            size_t num;                 \
            size_t capacity;            \
        };                              \
    }

#define da_init(v) darray_init(&v.da)

#define da_free(v) darray_free(&v.da)

#define da_alloc_size(v) (sizeof(*v.array) * v.num)

#define da_end(v) darray_end(sizeof(*v.array), &v.da)

#define da_reserve(v, capacity) \
        darray_reserve(sizeof(*v.array), &v.da, capacity)

#define da_resize(v, size) darray_resize(sizeof(*v.array), &v.da, size)

#define da_copy(dst, src) darray_copy(sizeof(*dst.array), &dst.da, &src.da)

#define da_copy_array(dst, src_array, n) \
        darray_copy_array(sizeof(*dst.array), &dst.da, src_array, n)

#define da_move(dst, src) darray_move(&dst.da, &src.da)

#define da_find(v, item, idx) darray_find(sizeof(*v.array), &v.da, item, idx)

#define da_push_back(v, item) darray_push_back(sizeof(*v.array), &v.da, item)

#define da_push_back_new(v) darray_push_back_new(sizeof(*v.array), &v.da)

#define da_push_back_array(dst, src_array, n) \
        darray_push_back_array(sizeof(*dst.array), &dst.da, src_array, n)

#define da_push_back_da(dst, src) \
        darray_push_back_darray(sizeof(*dst.array), &dst.da, &src.da)

#define da_insert(v, idx, item) \
        darray_insert(sizeof(*v.array), &v.da, idx, item)

#define da_insert_new(v, idx) darray_insert_new(sizeof(*v.array), &v.da, idx)

#define da_insert_array(dst, idx, src_array, n) \
        darray_insert_array(sizeof(*dst.array), &dst.da, idx, src_array, n)

#define da_insert_da(dst, idx, src) \
        darray_insert_darray(sizeof(*dst.array), &dst.da, idx, &src.da)

#define da_erase(dst, idx) darray_erase(sizeof(*dst.array), &dst.da, idx)

#define da_erase_item(dst, item) \
        darray_erase_item(sizeof(*dst.array), &dst.da, item)

#define da_erase_range(dst, from, to) \
        darray_erase_range(sizeof(*dst.array), &dst.da, from, to)

#define da_pop_back(dst) darray_pop_back(sizeof(*dst.array), &dst.da);

#define da_join(dst, src) darray_join(sizeof(*dst.array), &dst.da, &src.da)

#define da_split(dst1, dst2, src, idx) \
        darray_split(sizeof(*src.array), &dst1.da, &dst2.da, &src.da, idx)

#define da_move_item(v, from, to) \
        darray_move_item(sizeof(*v.array), &v.da, from, to)

#define da_swap(v, idx1, idx2) darray_swap(sizeof(*v.array), &v.da, idx1, idx2)


#ifdef __cplusplus
}
#endif
#endif
