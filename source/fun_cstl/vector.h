#ifndef _LIBCSTL_VECTOR_H
#define _LIBCSTL_VECTOR_H

#include <stddef.h> /* for size_t */
#include <stdbool.h>

typedef struct __vector vector_t;
typedef struct __vector_iterator * vector_iterator;
/**
*@brief 如果vector中存放的是指针，则需要提供此函数来释放存储的数据
*/
typedef void (*__vector_free_handler)(void *);

/**
*@brief 初始化一个vector，存储的是数据副本,因为初始化时就分配了空间。和map之类的不同
*/
vector_t *vector_init(size_t typesize);
/**
*@brief 设置释放函数
*/
void vector_set_free_handler(vector_t *v, __vector_free_handler vfree);
/**
*@brief return vector[index]
*/
void *vector_entry(vector_t *vector, size_t index);
/**
*@brief 访问第一个元素
*/
void *vector_front(vector_t *v);
/**
*@brief 访问最后一个元素
*/
void *vector_back(vector_t *v);
/**
*@brief 返回指向第一个元素的迭代器
*/
vector_iterator vector_begin(vector_t *v);
/**
*@brief 返回指向最后一个元素的迭代器
*/
vector_iterator vector_end(vector_t *v);
/**
*@brief 返回指向下一个元素的迭代器，如果成功，则返回真
*/
bool vector_next(vector_iterator iter);
/**
*@brief 返回指向上一个元素的迭代器，如果成功，则返回真
*/
bool vector_prev(vector_iterator iter);
/**
*@brief取出迭代器中的数据
*/
void *vector_get(vector_iterator iter);
/**
*@brief 释放一个迭代器
*/
void vector_free_iterator(vector_iterator iter);
/**
*@brief 判断vector是否为空
*/
bool vector_empty(vector_t *v);
/**返回容纳的元素数 */
size_t vector_size(vector_t *v);
/**返回当前存储空间能够容纳的元素数 */
size_t vector_capacity(vector_t *v);
/**预留存储空间 */
void vector_reserve(vector_t *v, size_t new_cap);
/**改变vector中容纳元素的个数 */
void vector_resize(vector_t *v, size_t count);
/**释放未使用的内存 */
void vector_shrink_to_fit(vector_t *v);
/**在pos之前插入一个元素 */
void vector_insert(vector_t *v, vector_iterator pos, const void *data);
/**在pos之前插入从data开始的count个元素 */
void vector_insert1(vector_t *v, vector_iterator pos, const void *data, size_t count);
/**删除pos处的元素 */
void vector_erase(vector_t *v, vector_iterator pos);
/**在vector末尾添加一个元素
 * 假如要添加元素e，则需要vector_push_back(v, &e)(这里&必不可少)
 * 不管e是一个值还是一个指针，都需如此
 */
void vector_push_back(vector_t *vector, const void *data);
/**从vector末尾弹出一个元素 */
void vector_pop_back(vector_t *v);
/**交换两个元素 */
void vector_swap(vector_t *v, size_t i, size_t j);
/**清空vector */
void vector_clear(vector_t *v);
/**释放vector */
void vector_free(vector_t *v);

#endif // _LIBCSTL_VECTOR_H
