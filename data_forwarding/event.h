#ifndef __EVENT_H
#define __EVENT_H

#include <stdint.h>

#include "queue.h"

#ifdef __cplusplus
extern "C" {
#endif

struct event_list;
struct event;


typedef int32_t (*eventAdd)(struct event_list *head,struct event *ev);
typedef int32_t (*eventRemove)(struct event_list *head,struct event *ev);

struct event_list
{
    struct list_head ev_list;
    int32_t     nodeNum;
};

struct event
{
    int32_t     fd;
    int32_t     mode;
};

struct event_ops
{
    const char *name;
    void *(*init)(void);
    void (*destroy)(void *);
    int32_t (*add)(void *,struct event *);
    int32_t (*mod)(void *,struct event *);
    int32_t (*del)(void *,struct event *);
    int32_t (*run)(struct eventOpe *,void *,struct event *);
};


/* event list ops */
int32_t eventListInit(struct event_list *a_list);
int32_t eventListAdd(struct event_list *a_head,struct event *a_ev);
int32_t eventListDel(struct event_list *a_head,struct event *a_ev);



#ifdef __cplusplus
}
#endif

#endif

