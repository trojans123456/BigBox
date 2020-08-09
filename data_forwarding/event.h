#ifndef __EVENT_H
#define __EVENT_H

#include <stdint.h>

#include "queue.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EVENT_READ      (1UL << 0)
#define EVENT_WRITE     (1UL << 1)
#define EVENT_TIIMEOUT  (1UL << 2)
#define EVENT_ERROR     (1UL << 3)

#define IN_MINHEAP      0X01
#define OUT_MINHEAP     0X02

struct event_list;
struct event;
struct reactor;

typedef int32_t (*eventAddCb)(struct event_list *head,struct event *ev);
typedef int32_t (*eventRemoveCb)(struct event_list *head,struct event *ev);

typedef void (*eventExecCb)(struct event *,int ,void *);


struct eventOpe
{
    struct event_list *activeQueue;
    eventAddCb evAdd;
};

struct event_list
{
    struct list_head ev_list;
    int32_t     nodeNum;
};

struct event
{
    int32_t     fd;
    int32_t     mode;
    int32_t     ev_mode; /* event */
    struct list_head head;

    eventExecCb   evExec;
    void          *args;

    int8_t        minHeapFlag;/* minheap key */
    int32_t       timeDelay; /* timeout time delay */
};

struct event_ops
{
    const char *name;
    void *(*init)(void);
    void (*destroy)(void *);
    int32_t (*add)(void *,struct event *);
    int32_t (*mod)(void *,struct event *);
    int32_t (*del)(void *,struct event *);
    int32_t (*run)(struct eventOpe *,void *,int32_t timeout);
};

struct reactor
{
    struct event_ops *sysOps;/* method of event ops */
    struct event_list   activeQueue;
    struct event_list   timeoutQueue;
    struct minheap      *minheap;
    struct eventOpe     evOper;
    void                *listen_fd;
    int32_t             maxConNum;
};

struct reactor *reactorInit(int32_t maxConNum);
void reactorDestroy(struct reactor *a_this);

int reactorRun(struct reactor *a_this);

int eventInit(struct event *ev,int fd,int mode,eventExecCb ev_exe,void *args);

int eventAddListen(struct reactor *a_this,struct event *ev);
int eventRmListen(struct reactor *a_this,struct event *ev);
int eventModListen(struct reactor *a_this,struct event *ev,int32_t mode);

int eventAddTimeout(struct event *ev,int timeout);

/* event list ops */
int32_t eventListInit(struct event_list *a_list);
int32_t eventListAdd(struct event_list *a_head,struct event *a_ev);
int32_t eventListDel(struct event_list *a_head,struct event *a_ev);



#ifdef __cplusplus
}
#endif

#endif

