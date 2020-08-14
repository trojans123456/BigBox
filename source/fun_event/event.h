#ifndef __EVENT_H
#define __EVENT_H

#include <stdint.h>
#include <stdlib.h>

#include <pthread.h>
#include <sys/timerfd.h>

#ifdef __cplusplus
extern "C" {
#endif


enum event_timer_type
{
    TIMER_ONESHOT = 0, /*执行一次*/
    TIMER_PERSIST /*周期*/
};

enum event_flags
{
    EVENT_TIMEOUT  = 1<<0,
    EVENT_READ     = 1<<1,
    EVENT_WRITE    = 1<<2,
    EVENT_SIGNAL   = 1<<3,
    EVENT_PERSIST  = 1<<4,
    EVENT_ET       = 1<<5,
    EVENT_FINALIZE = 1<<6,
    EVENT_CLOSED   = 1<<7,
    EVENT_ERROR    = 1<<8,
    EVENT_EXCEPT   = 1<<9,
};

struct event_cbs
{
    void (*ev_in)(int fd, void *arg);
    void (*ev_out)(int fd, void *arg);
    void (*ev_err)(int fd, void *arg);
    void (*ev_timer)(int fd, void *arg);

    struct itimerspec itimer; /* for timerfd */

    void *args;
};

struct event
{
    int evfd;
    int flags;
    struct event_cbs *evcb;
};

struct event_base;

struct event_ops
{
    void *(*init)();
    void (*deinit)(void *ctx);
    int (*add)(struct event_base *eb, struct event *e);
    int (*del)(struct event_base *eb, struct event *e);
    int (*dispatch)(struct event_base *eb, struct timeval *tv);
};

struct event_base
{
    void *ctx;
    int loop;
    int rfd;
    int wfd;
    pthread_t tid;
    const struct event_ops *evop;
};

struct event_base *event_base_create();
void event_base_destroy(struct event_base *);
int event_base_loop(struct event_base *);
int event_base_loop_start(struct event_base *eb);
int event_base_loop_stop(struct event_base *eb);
void event_base_loop_break(struct event_base *);
int event_base_wait(struct event_base *eb);
void event_base_signal(struct event_base *eb);

struct event *event_create(int fd,
        void (ev_in)(int, void *),
        void (ev_out)(int, void *),
        void (ev_err)(int, void *),
        void *args);

void event_destroy(struct event *e);
int event_add(struct event_base *eb, struct event *e);
int event_del(struct event_base *eb, struct event *e);
struct event *event_timer_create(time_t msec,
        enum event_timer_type type,
        void (ev_timer)(int, void *),
        void *args);

#ifdef __cplusplus
}
#endif

#endif
