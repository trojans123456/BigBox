#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "event.h"

extern const struct event_ops selectops;
extern const struct event_ops pollops;
extern const struct event_ops epollops;

static const struct event_ops *eventops[] =
{
    //&selectops,
    //&pollops,
    &epollops,
    NULL
};

static void event_in(int fd,void *arg)
{

}

struct event_base *event_base_create()
{
    int i;
    int fds[2];
    struct event *e;
    struct event_base *eb = NULL;

    if (pipe(fds)) {
        perror("pipe failed");
        return NULL;
    }

    eb = (struct event_base *)calloc(1, sizeof(struct event_base));
    if (!eb) {
        printf("malloc event_base failed!\n");
        close(fds[0]);
        close(fds[1]);
        return NULL;
    }

    for (i = 0; eventops[i]; i++) {
        eb->ctx = eventops[i]->init();
        eb->evop = eventops[i];
    }
    eb->loop = 1;
    eb->rfd = fds[0];
    eb->wfd = fds[1];

    fcntl(fds[0], F_SETFL, fcntl(fds[0], F_GETFL) | O_NONBLOCK);

    e = event_create(eb->rfd, event_in, NULL, NULL, NULL);
    event_add(eb, e);
    return eb;
}

void event_base_destroy(struct event_base *eb)
{
    if (!eb) {
        return;
    }
    event_base_loop_break(eb);
    close(eb->rfd);
    close(eb->wfd);
    eb->evop->deinit(eb->ctx);
    free(eb);
}

int event_base_wait(struct event_base *eb)
{
    const struct event_ops *evop = eb->evop;
    return evop->dispatch(eb, NULL);
}

int event_base_loop(struct event_base *eb)
{
    const struct event_ops *evop = eb->evop;
    int ret;
    while (eb->loop) {
        ret = evop->dispatch(eb, NULL);
        if (ret == -1) {
            printf("dispatch failed\n");
        }
    }
    return 0;
}


static void *_event_base_loop(void *arg)
{
    struct event_base *eb = (struct event_base *)arg;
    event_base_loop(eb);
    return NULL;
}

int event_base_loop_start(struct event_base *eb)
{
    pthread_create(&eb->tid, NULL, _event_base_loop, eb);
    return 0;
}

int event_base_loop_stop(struct event_base *eb)
{
    event_base_loop_break(eb);
    pthread_join(eb->tid, NULL);
    return 0;
}

void event_base_loop_break(struct event_base *eb)
{
    char buf[1];
    buf[0] = 0;
    eb->loop = 0;
    if (1 != write(eb->wfd, buf, 1)) {
        perror("write error");
    }
}

void event_base_signal(struct event_base *eb)
{
    char buf[1];
    buf[0] = 0;
    if (1 != write(eb->wfd, buf, 1)) {
        perror("write error");
    }
}

struct event *event_create(int fd,
        void (ev_in)(int, void *),
        void (ev_out)(int, void *),
        void (ev_err)(int, void *),
        void *args)
{
    int flags = 0;
    struct event_cbs *evcb;
    struct event *e = (struct event *)calloc(1, sizeof(struct event));
    if (!e) {
        printf("malloc gevent failed!\n");
        return NULL;
    }
    evcb = (struct event_cbs *)calloc(1, sizeof(struct event_cbs));
    if (!evcb) {
        printf("malloc gevent failed!\n");
        return NULL;
    }
    evcb->ev_in = ev_in;
    evcb->ev_out = ev_out;
    evcb->ev_err = ev_err;
    evcb->ev_timer = NULL;
    evcb->args = args;
    if (ev_in) {
        flags |= EVENT_READ;
    }
    if (ev_out) {
        flags |= EVENT_WRITE;
    }
    if (ev_err) {
        flags |= EVENT_ERROR;
    }

    flags |= EVENT_PERSIST;
    e->evfd = fd;
    e->flags = flags;
    e->evcb = evcb;

    return e;
}

struct event *event_timer_create(time_t msec,
        enum event_timer_type type,
        void (ev_timer)(int, void *),
        void *args)
{

    enum event_flags flags = 0;
    struct event_cbs *evcb;
    int fd;
    time_t sec = msec/1000;
    long nsec = (msec-sec*1000)*1000000;

    struct event *e = (struct event *)calloc(1, sizeof(struct event));
    if (!e) {
        printf("malloc gevent failed!\n");
        goto failed;
    }
    evcb = (struct event_cbs *)calloc(1, sizeof(struct event_cbs));
    if (!evcb) {
        printf("malloc gevent failed!\n");
        goto failed;
    }
    evcb->ev_timer = ev_timer;
    evcb->ev_in = NULL;
    evcb->ev_out = NULL;
    evcb->ev_err = NULL;
    evcb->args = args;
    flags = EVENT_READ;
    if (type == TIMER_PERSIST) {
        flags |= EVENT_PERSIST;
    } else if (type == TIMER_ONESHOT) {
        flags &= ~EVENT_PERSIST;
    }

    fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
    if (fd == -1) {
        printf("timerfd_create failed %d\n", errno);
        goto failed;
    }


    evcb->itimer.it_value.tv_sec = sec;
    evcb->itimer.it_value.tv_nsec = nsec;
    evcb->itimer.it_interval.tv_sec = sec;
    evcb->itimer.it_interval.tv_nsec = nsec;
    if (0 != timerfd_settime(fd, 0, &evcb->itimer, NULL)) {
        printf("timerfd_settime failed!\n");
        goto failed;
    }

    e->evfd = fd;
    e->flags = flags;
    e->evcb = evcb;
    return e;

failed:
    if (e->evcb) free(e->evcb);
    if (e) free(e);

    return NULL;
}

void event_destroy(struct event *e)
{
    if (!e)
        return;
    if (e->evcb)
        free(e->evcb);
    free(e);
}

int event_add(struct event_base *eb, struct event *e)
{
    if (!e || !eb) {
        printf("%s:%d paraments is NULL\n", __func__, __LINE__);
        return -1;
    }
    return eb->evop->add(eb, e);
}

int event_del(struct event_base *eb, struct event *e)
{
    if (!e || !eb) {
        printf("%s:%d paraments is NULL\n", __func__, __LINE__);
        return -1;
    }
    return eb->evop->del(eb, e);
}
