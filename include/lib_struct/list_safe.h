#ifndef __LIST_SAFE_H
#define __LIST_SAFE_H

#include <stdbool.h>
#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif


struct safe_list;
struct safe_list_iterator;

struct safe_list
{
    struct list_head list;
    struct safe_list_iterator *i;
};


int safe_list_for_each(struct safe_list *list,
               int (*cb)(void *ctx, struct safe_list *list),
               void *ctx);

void safe_list_add(struct safe_list *list, struct safe_list *head);
void safe_list_add_first(struct safe_list *list, struct safe_list *head);
void safe_list_del(struct safe_list *list);

#define INIT_SAFE_LIST(_head) \
    do { \
        INIT_LIST_HEAD(_head.list); \
        (_head)->i = NULL; \
    } while (0)

#define SAFE_LIST_INIT(_name) { LIST_HEAD_INIT(_name.list), NULL }
#define SAFE_LIST(_name)	struct safe_list _name = SAFE_LIST_INIT(_name)

static inline bool safe_list_empty(struct safe_list *head)
{
    return list_empty(&head->list);
}

#ifdef __cplusplus
}
#endif

#endif

