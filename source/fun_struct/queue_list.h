#ifndef QUEUE_LIST_H
#define QUEUE_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

/*
 *@brief 单链表
 */
#define LW_SLIST_HEAD(name, type)						\
struct name {								\
    struct type *slh_first;	/* first element */			\
}

#define	LW_SLIST_HEAD_INITIALIZER(head)					\
    { NULL }

#define LW_SLIST_ENTRY(type)						\
struct {								\
    struct type *sle_next;	/* next element */			\
}


/*
 *@brief 单链表访问方法
 */
#define	LW_SLIST_FIRST(head)	((head)->slh_first)
#define	LW_SLIST_END(head)		NULL
#define	LW_SLIST_EMPTY(head)	(LW_SLIST_FIRST(head) == LW_SLIST_END(head))
#define	LW_SLIST_NEXT(elm, field)	((elm)->field.sle_next)

#define	LW_SLIST_FOREACH(var, head, field)					\
    for((var) = SLIST_FIRST(head);					\
        (var) != SLIST_END(head);					\
        (var) = SLIST_NEXT(var, field))

/*
 *@brief 单链表的功能.
 */
#define	LW_SLIST_INIT(head) {						\
    LW_SLIST_FIRST(head) = LW_SLIST_END(head);				\
}

#define	LW_SLIST_INSERT_AFTER(slistelm, elm, field) do {			\
    (elm)->field.sle_next = (slistelm)->field.sle_next;		\
    (slistelm)->field.sle_next = (elm);				\
} while (0)

#define	LW_SLIST_INSERT_HEAD(head, elm, field) do {			\
    (elm)->field.sle_next = (head)->slh_first;			\
    (head)->slh_first = (elm);					\
} while (0)

#define	LW_SLIST_REMOVE_HEAD(head, field) do {				\
    (head)->slh_first = (head)->slh_first->field.sle_next;		\
} while (0)

/*
 * 链表定义.
 */
#define LW_LIST_HEAD(name, type)						\
struct name {								\
    struct type *lh_first;	/* first element */			\
}

#define LW_LIST_HEAD_INITIALIZER(head)					\
    { NULL }

#define LW_LIST_ENTRY(type)						\
struct {								\
    struct type *le_next;	/* next element */			\
    struct type **le_prev;	/* address of previous next element */	\
}

/*
 * 链表访问方法
 */
#define	LW_LIST_FIRST(head)		((head)->lh_first)
#define	LW_LIST_END(head)			NULL
#define	LW_LIST_EMPTY(head)		(LW_LIST_FIRST(head) == LW_LIST_END(head))
#define	LW_LIST_NEXT(elm, field)		((elm)->field.le_next)

#define LW_LIST_FOREACH(var, head, field)					\
    for((var) = LW_LIST_FIRST(head);					\
        (var)!= LW_LIST_END(head);					\
        (var) = LW_LIST_NEXT(var, field))

/*
 * 双向链表函数.
 */
#define	LW_LIST_INIT(head) do {						\
    LIST_FIRST(head) = LW_LIST_END(head);				\
} while (0)

#define LW_LIST_INSERT_AFTER(listelm, elm, field) do {			\
    if (((elm)->field.le_next = (listelm)->field.le_next) != NULL)	\
        (listelm)->field.le_next->field.le_prev =		\
            &(elm)->field.le_next;				\
    (listelm)->field.le_next = (elm);				\
    (elm)->field.le_prev = &(listelm)->field.le_next;		\
} while (0)

#define	LW_LIST_INSERT_BEFORE(listelm, elm, field) do {			\
    (elm)->field.le_prev = (listelm)->field.le_prev;		\
    (elm)->field.le_next = (listelm);				\
    *(listelm)->field.le_prev = (elm);				\
    (listelm)->field.le_prev = &(elm)->field.le_next;		\
} while (0)

#define LW_LIST_INSERT_HEAD(head, elm, field) do {				\
    if (((elm)->field.le_next = (head)->lh_first) != NULL)		\
        (head)->lh_first->field.le_prev = &(elm)->field.le_next;\
    (head)->lh_first = (elm);					\
    (elm)->field.le_prev = &(head)->lh_first;			\
} while (0)

#define LW_LIST_REMOVE(elm, field) do {					\
    if ((elm)->field.le_next != NULL)				\
        (elm)->field.le_next->field.le_prev =			\
            (elm)->field.le_prev;				\
    *(elm)->field.le_prev = (elm)->field.le_next;			\
} while (0)

#define LW_LIST_REPLACE(elm, elm2, field) do {				\
    if (((elm2)->field.le_next = (elm)->field.le_next) != NULL)	\
        (elm2)->field.le_next->field.le_prev =			\
            &(elm2)->field.le_next;				\
    (elm2)->field.le_prev = (elm)->field.le_prev;			\
    *(elm2)->field.le_prev = (elm2);				\
} while (0)

/*
 * 简单队列，只能向前移动.
 */
#define LW_SIMPLEQ_HEAD(name, type)					\
struct name {								\
    struct type *sqh_first;	/* first element */			\
    struct type **sqh_last;	/* addr of last next element */		\
}

#define LW_SIMPLEQ_HEAD_INITIALIZER(head)					\
    { NULL, &(head).sqh_first }

/*要嵌入式到其他结构体中  LW_SIMPLAEQ_ENTRY(xx) aa*/
#define LW_SIMPLEQ_ENTRY(type)						\
struct {								\
    struct type *sqe_next;	/* next element */			\
}

/*
 * 简单队列的访问方法.
 */
#define	LW_SIMPLEQ_FIRST(head)	    ((head)->sqh_first)
#define	LW_SIMPLEQ_END(head)	    NULL
#define	LW_SIMPLEQ_EMPTY(head)	    (LW_SIMPLEQ_FIRST(head) == LW_SIMPLEQ_END(head))
#define	LW_SIMPLEQ_NEXT(elm, field)    ((elm)->field.sqe_next)

#define LW_SIMPLEQ_FOREACH(var, head, field)				\
    for((var) = LW_SIMPLEQ_FIRST(head);				\
        (var) != LW_SIMPLEQ_END(head);					\
        (var) = LW_SIMPLEQ_NEXT(var, field))

/*
 * 简单队列的函数.
 */
#define	LW_SIMPLEQ_INIT(head) do {						\
    (head)->sqh_first = NULL;					\
    (head)->sqh_last = &(head)->sqh_first;				\
} while (0)

#define LW_SIMPLEQ_INSERT_HEAD(head, elm, field) do {			\
    if (((elm)->field.sqe_next = (head)->sqh_first) == NULL)	\
        (head)->sqh_last = &(elm)->field.sqe_next;		\
    (head)->sqh_first = (elm);					\
} while (0)

#define LW_SIMPLEQ_INSERT_TAIL(head, elm, field) do {			\
    (elm)->field.sqe_next = NULL;					\
    *(head)->sqh_last = (elm);					\
    (head)->sqh_last = &(elm)->field.sqe_next;			\
} while (0)

#define LW_SIMPLEQ_INSERT_AFTER(head, listelm, elm, field) do {		\
    if (((elm)->field.sqe_next = (listelm)->field.sqe_next) == NULL)\
        (head)->sqh_last = &(elm)->field.sqe_next;		\
    (listelm)->field.sqe_next = (elm);				\
} while (0)

#define LW_SIMPLEQ_REMOVE_HEAD(head, elm, field) do {			\
    if (((head)->sqh_first = (elm)->field.sqe_next) == NULL)	\
        (head)->sqh_last = &(head)->sqh_first;			\
} while (0)

/*
 * 单链表的队列。允许前后访问。随意删除随意添加.
 */
//初始化tailq头。LW_TAILQ_HEAD(tail_head,(struct type)) head
#define LW_TAILQ_HEAD(name, type)						\
struct name {								\
    struct type *tqh_first;	/* first element */			\
    struct type **tqh_last;	/* addr of last next element */		\
}

#define LW_TAILQ_HEAD_INITIALIZER(head)					\
    { NULL, &(head).tqh_first }

#define LW_TAILQ_ENTRY(type)						\
struct {								\
    struct type *tqe_next;	/* next element */			\
    struct type **tqe_prev;	/* address of previous next element */	\
}

/*
 * tail queue access methods
 */
#define	LW_TAILQ_FIRST(head)		((head)->tqh_first)
#define	LW_TAILQ_END(head)			NULL
#define	LW_TAILQ_NEXT(elm, field)		((elm)->field.tqe_next)
#define LW_TAILQ_LAST(head, headname)					\
    (*(((struct headname *)((head)->tqh_last))->tqh_last))
/* XXX */
#define LW_TAILQ_PREV(elm, headname, field)				\
    (*(((struct headname *)((elm)->field.tqe_prev))->tqh_last))
#define	LW_TAILQ_EMPTY(head)						\
    (TAILQ_FIRST(head) == TAILQ_END(head))

#define LW_TAILQ_FOREACH(var, head, field)					\
    for((var) = LW_TAILQ_FIRST(head);					\
        (var) != LW_TAILQ_END(head);					\
        (var) = LW_TAILQ_NEXT(var, field))

#define LW_TAILQ_FOREACH_REVERSE(var, head, headname, field)		\
    for((var) = TAILQ_LAST(head, headname);				\
        (var) != TAILQ_END(head);					\
        (var) = TAILQ_PREV(var, headname, field))

/*
 * Tail queue functions.
 */
#define	LW_TAILQ_INIT(head) do {						\
    (head)->tqh_first = NULL;					\
    (head)->tqh_last = &(head)->tqh_first;				\
} while (0)

#define LW_TAILQ_INSERT_HEAD(head, elm, field) do {			\
    if (((elm)->field.tqe_next = (head)->tqh_first) != NULL)	\
        (head)->tqh_first->field.tqe_prev =			\
            &(elm)->field.tqe_next;				\
    else								\
        (head)->tqh_last = &(elm)->field.tqe_next;		\
    (head)->tqh_first = (elm);					\
    (elm)->field.tqe_prev = &(head)->tqh_first;			\
} while (0)

#define LW_TAILQ_INSERT_TAIL(head, elm, field) do {			\
    (elm)->field.tqe_next = NULL;					\
    (elm)->field.tqe_prev = (head)->tqh_last;			\
    *(head)->tqh_last = (elm);					\
    (head)->tqh_last = &(elm)->field.tqe_next;			\
} while (0)

#define LW_TAILQ_INSERT_AFTER(head, listelm, elm, field) do {		\
    if (((elm)->field.tqe_next = (listelm)->field.tqe_next) != NULL)\
        (elm)->field.tqe_next->field.tqe_prev =			\
            &(elm)->field.tqe_next;				\
    else								\
        (head)->tqh_last = &(elm)->field.tqe_next;		\
    (listelm)->field.tqe_next = (elm);				\
    (elm)->field.tqe_prev = &(listelm)->field.tqe_next;		\
} while (0)

#define	LW_TAILQ_INSERT_BEFORE(listelm, elm, field) do {			\
    (elm)->field.tqe_prev = (listelm)->field.tqe_prev;		\
    (elm)->field.tqe_next = (listelm);				\
    *(listelm)->field.tqe_prev = (elm);				\
    (listelm)->field.tqe_prev = &(elm)->field.tqe_next;		\
} while (0)

#define LW_TAILQ_REMOVE(head, elm, field) do {				\
    if (((elm)->field.tqe_next) != NULL)				\
        (elm)->field.tqe_next->field.tqe_prev =			\
            (elm)->field.tqe_prev;				\
    else								\
        (head)->tqh_last = (elm)->field.tqe_prev;		\
    *(elm)->field.tqe_prev = (elm)->field.tqe_next;			\
} while (0)

#define LW_TAILQ_REPLACE(head, elm, elm2, field) do {			\
    if (((elm2)->field.tqe_next = (elm)->field.tqe_next) != NULL)	\
        (elm2)->field.tqe_next->field.tqe_prev =		\
            &(elm2)->field.tqe_next;				\
    else								\
        (head)->tqh_last = &(elm2)->field.tqe_next;		\
    (elm2)->field.tqe_prev = (elm)->field.tqe_prev;			\
    *(elm2)->field.tqe_prev = (elm2);				\
} while (0)

/*
 * 双向循环队列 (外部加锁。或一个线程pushin 另一个pushout)
 */
#define LW_CIRCLEQ_HEAD(name, type)					\
struct name {								\
    struct type *cqh_first;		/* first element */		\
    struct type *cqh_last;		/* last element */		\
}

#define LW_CIRCLEQ_HEAD_INITIALIZER(head)					\
    { LW_CIRCLEQ_END(&head), LW_CIRCLEQ_END(&head) }

#define LW_CIRCLEQ_ENTRY(type)						\
struct {								\
    struct type *cqe_next;		/* next element */		\
    struct type *cqe_prev;		/* previous element */		\
}

/*
 * Circular queue access methods
 */
#define	LW_CIRCLEQ_FIRST(head)		((head)->cqh_first)
#define	LW_CIRCLEQ_LAST(head)		((head)->cqh_last)
#define	LW_CIRCLEQ_END(head)		((void *)(head))
#define	LW_CIRCLEQ_NEXT(elm, field)	((elm)->field.cqe_next)
#define	LW_CIRCLEQ_PREV(elm, field)	((elm)->field.cqe_prev)
#define	LW_CIRCLEQ_EMPTY(head)						\
    (LW_CIRCLEQ_FIRST(head) == LW_CIRCLEQ_END(head))

#define LW_CIRCLEQ_FOREACH(var, head, field)				\
    for((var) = LW_CIRCLEQ_FIRST(head);				\
        (var) != LW_CIRCLEQ_END(head);					\
        (var) = LW_CIRCLEQ_NEXT(var, field))

#define LW_CIRCLEQ_FOREACH_REVERSE(var, head, field)			\
    for((var) = LW_CIRCLEQ_LAST(head);					\
        (var) != LW_CIRCLEQ_END(head);					\
        (var) = LW_CIRCLEQ_PREV(var, field))

/*
 * Circular queue functions.
 */
#define	LW_CIRCLEQ_INIT(head) do {						\
    (head)->cqh_first = LW_CIRCLEQ_END(head);				\
    (head)->cqh_last = LW_CIRCLEQ_END(head);				\
} while (0)

#define LW_CIRCLEQ_INSERT_AFTER(head, listelm, elm, field) do {		\
    (elm)->field.cqe_next = (listelm)->field.cqe_next;		\
    (elm)->field.cqe_prev = (listelm);				\
    if ((listelm)->field.cqe_next == LW_CIRCLEQ_END(head))		\
        (head)->cqh_last = (elm);				\
    else								\
        (listelm)->field.cqe_next->field.cqe_prev = (elm);	\
    (listelm)->field.cqe_next = (elm);				\
} while (0)

#define LW_CIRCLEQ_INSERT_BEFORE(head, listelm, elm, field) do {		\
    (elm)->field.cqe_next = (listelm);				\
    (elm)->field.cqe_prev = (listelm)->field.cqe_prev;		\
    if ((listelm)->field.cqe_prev == LW_CIRCLEQ_END(head))		\
        (head)->cqh_first = (elm);				\
    else								\
        (listelm)->field.cqe_prev->field.cqe_next = (elm);	\
    (listelm)->field.cqe_prev = (elm);				\
} while (0)

#define LW_CIRCLEQ_INSERT_HEAD(head, elm, field) do {			\
    (elm)->field.cqe_next = (head)->cqh_first;			\
    (elm)->field.cqe_prev = LW_CIRCLEQ_END(head);			\
    if ((head)->cqh_last == LW_CIRCLEQ_END(head))			\
        (head)->cqh_last = (elm);				\
    else								\
        (head)->cqh_first->field.cqe_prev = (elm);		\
    (head)->cqh_first = (elm);					\
} while (0)

#define LW_CIRCLEQ_INSERT_TAIL(head, elm, field) do {			\
    (elm)->field.cqe_next = LW_CIRCLEQ_END(head);			\
    (elm)->field.cqe_prev = (head)->cqh_last;			\
    if ((head)->cqh_first == LW_CIRCLEQ_END(head))			\
        (head)->cqh_first = (elm);				\
    else								\
        (head)->cqh_last->field.cqe_next = (elm);		\
    (head)->cqh_last = (elm);					\
} while (0)

#define	LW_CIRCLEQ_REMOVE(head, elm, field) do {				\
    if ((elm)->field.cqe_next == CIRCLEQ_END(head))			\
        (head)->cqh_last = (elm)->field.cqe_prev;		\
    else								\
        (elm)->field.cqe_next->field.cqe_prev =			\
            (elm)->field.cqe_prev;				\
    if ((elm)->field.cqe_prev == LW_CIRCLEQ_END(head))			\
        (head)->cqh_first = (elm)->field.cqe_next;		\
    else								\
        (elm)->field.cqe_prev->field.cqe_next =			\
            (elm)->field.cqe_next;				\
} while (0)

#define LW_CIRCLEQ_REPLACE(head, elm, elm2, field) do {			\
    if (((elm2)->field.cqe_next = (elm)->field.cqe_next) ==		\
        CIRCLEQ_END(head))						\
        (head).cqh_last = (elm2);				\
    else								\
        (elm2)->field.cqe_next->field.cqe_prev = (elm2);	\
    if (((elm2)->field.cqe_prev = (elm)->field.cqe_prev) ==		\
        CIRCLEQ_END(head))						\
        (head).cqh_first = (elm2);				\
    else								\
        (elm2)->field.cqe_prev->field.cqe_next = (elm2);	\
} while (0)


#ifdef __cplusplus
}
#endif

#endif //
