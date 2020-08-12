#ifndef __WIN32
    #include <sys/epoll.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <signal.h>
#endif
#include <stdlib.h>
#include <string.h>

#include "event.h"


#define MAX_FD  32
#define EVENT_MAX_EVENTS    10

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

struct event_fd_event
{
    struct event_fd *fd;
    unsigned int events;
};
static struct event_fd_event cur_fds[EVENT_MAX_EVENTS];


static int poll_fd = -1;
static struct list_head timeout_head = LIST_HEAD_INIT(timeout_head);

static int event_cancelled = 0;


/***************** epoll *******************/
static int event_init_pollfd(void)
{
    if(poll_fd >= 0)
        return 0;

    poll_fd = epoll_create(MAX_FD);
    if(poll_fd < 0)
        return -1;

    fcntl(poll_fd,F_SETFD,fcntl(poll_fd,F_GETFD) | FD_CLOEXEC);
    return 0;
}


static int register_poll(struct event_fd *fd,uint32_t flags)
{
    struct epoll_event ev;
    int op = fd->registered ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;

    memset(&ev,0,sizeof(struct epoll_event));

    if(flags & EVENT_READ)
    {
        ev.events |= (EPOLLIN | EPOLLRDHUP);
    }
    if(flags & EVENT_WRITE)
    {
        ev.events |= EPOLLOUT;
    }
    if(flags & EVENT_EDGE_TRIGGER)
    {
        ev.events |= EPOLLET;
    }

    ev.data.ptr = fd;

    fd->flags = flags;

    return epoll_ctl(poll_fd,op,fd->fd,&ev);
}

static int __event_fd_delete(struct event_fd *fd)
{
    fd->fd = 0;
    return epoll_ctl(poll_fd,EPOLL_CTL_DEL,fd->fd,0);
}



static int event_fetch_events(int timeout)
{
    int n, nfds;
    struct epoll_event events[EVENT_MAX_EVENTS];
    struct event_fd *u = NULL;

    struct event_fd_event *cur = NULL;

    nfds = epoll_wait(poll_fd, events, ARRAY_SIZE(events), timeout);
    if(nfds == -1)
    {
        return -1;
    }
    if(nfds == 0)
    {
        /* timeout */
        return 0;
    }

    for (n = 0; n < nfds; ++n) {
        cur = &cur_fds[n];
        u = events[n].data.ptr;
        unsigned int ev = 0;

        cur->fd = u;
        if (!u)
            continue;

        if (events[n].events & (EPOLLERR|EPOLLHUP)) {
            u->error = true;
            //if (!(u->flags & EVENT_ERROR))
                event_fd_delete(u);
        }

        if(!(events[n].events & (EPOLLRDHUP|EPOLLIN|EPOLLOUT|EPOLLERR|EPOLLHUP))) {
            cur->fd = NULL;
            continue;
        }
        /*对端关闭 */
        if(events[n].events & EPOLLRDHUP)
            u->eof = true;

        if(events[n].events & EPOLLIN)
            ev |= EVENT_READ;

        if(events[n].events & EPOLLOUT)
            ev |= EVENT_WRITE;

        cur->events = ev;
    }

    return nfds;
}


int event_fd_add(struct event_fd *sock,uint32_t flags)
{
    unsigned int fl;
    int ret;

    if(!(flags & (EVENT_READ | EVENT_WRITE)))
    {
        return event_fd_delete(sock);
    }

    if(!sock->registered && !(flags & EVENT_BLOCKING))
    {
        fl = fcntl(sock->fd,F_GETFL,0);
        fl |= O_NONBLOCK;
        fcntl(sock->fd,F_SETFL,fl);
    }

    ret = register_poll(sock,flags);
    if(ret < 0)
    {
        goto out;
    }

    sock->registered = true;
    sock->eof = false;
    sock->error = false;

out:
    return ret;
}

int event_fd_delete(struct event_fd *fd)
{
    if(!fd->registered)
        return 0;

    fd->registered = false;
    return __event_fd_delete(fd);
}

static int tv_diff(struct timeval *t1,struct timeval *t2)
{
    return (t1->tv_sec - t2->tv_sec) * 1000 +
            (t1->tv_usec - t2->tv_usec) / 1000;
}


int event_timeout_add(struct event_timeout *ev_tm)
{
    struct event_timeout *tmp;
    struct list_head *h = &timeout_head;

    if(ev_tm->pending)
        return -1;

    list_for_each_entry(tmp,&timeout_head,list) {

        if(tv_diff(&tmp->time,&ev_tm->time) > 0)
        {
            h = &tmp->list;
            break;
        }
    }

    list_add_tail(&ev_tm->list,h);
    ev_tm->pending = true;

    return 0;
}

static void event_gettime(struct timeval *tv)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC,&ts);
    tv->tv_sec = ts.tv_sec;
    tv->tv_usec = ts.tv_nsec / 1000;
}

int event_timeout_set(struct event_timeout *ev_tm, int msecs)
{
    struct timeval *time = &ev_tm->time;

    if(ev_tm->pending)
    {
        event_timeout_cancel(ev_tm);
    }

    event_gettime(time);

    time->tv_sec += msecs / 1000;
    time->tv_usec += (msecs % 1000) * 1000;

    if(time->tv_usec > 1000000)
    {
        time->tv_sec++;
        time->tv_usec -= 1000000;
    }

    return event_timeout_add(ev_tm);
}

int event_timeout_cancel(struct event_timeout *ev_tm)
{
    if(!ev_tm->pending)
        return -1;

    list_del(&ev_tm->list);
    ev_tm->pending = false;

    return 0;
}

int event_timeout_remaining(struct event_timeout *ev_tm)
{
    struct timeval now;
    if(!ev_tm->pending)
        return -1;

    event_gettime(&now);

    return tv_diff(&ev_tm->time,&now);
}



static void event_ignore_signal(int signum,bool ignore)
{
    /* ignore signals */
    struct sigaction s;
    void *new_handler = NULL;

    sigaction(signum, NULL, &s);

    if (ignore) {
        if (s.sa_handler == SIG_DFL) /* Ignore only if there isn't any custom handler */
            new_handler = SIG_IGN;
    } else {
        if (s.sa_handler == SIG_IGN) /* Restore only if noone modified our SIG_IGN */
            new_handler = SIG_DFL;
    }

    if (new_handler) {
        s.sa_handler = new_handler;
        s.sa_flags = 0;
        sigaction(signum, &s, NULL);
    }
}

/**
 * @brief 初始化事件循环
 * @return
 */
int event_loop_init(void)
{
    if(event_init_pollfd() < 0)
        return -1;

    event_ignore_signal(SIGPIPE,true);

    return 0;
}

static void event_process_timeouts(struct timeval *tv)
{
    struct event_timeout *t;

    while(!list_empty(&timeout_head))
    {
        t = list_first_entry(&timeout_head,struct event_timeout,list);
        if(tv_diff(&t->time,tv) > 0)
        {
            break;
        }

        event_timeout_cancel(t);
        if(t->cb)
        {
            t->cb(t);
        }
    }
}

static int event_get_next_timeout(struct timeval *tv)
{
    struct event_timeout *timeout;
    int diff;

    if(list_empty(&timeout_head))
        return -1;

    timeout = list_first_entry(&timeout_head,struct event_timeout,list);
    diff = tv_diff(&timeout->time,tv);
    if(diff < 0)
        return 0;

    return diff;
}

static void event_process_events(int timeout)
{
    struct event_fd_event *cur;
    struct event_fd *fd;
    unsigned int events;
    static int cur_fd = 0,cur_nfds = 0;


    cur_nfds = event_fetch_events(timeout);

    while(cur_nfds > 0)
    {
        cur = &cur_fds[cur_fd++];
        cur_nfds--;

        fd = cur->fd;
        events = cur->events;
        if(!fd)
            continue;

        if(!fd->cb)
            continue;

        fd->cb(fd,events);
    }
}

/**
 * @brief 事件循环入口
 */
void event_loop_run(void)
{
    int next_time = 0;
    struct timeval tv;

    while(!event_cancelled)
    {
        event_gettime(&tv);
        event_process_timeouts(&tv);

        if(event_cancelled)
            break;

        event_gettime(&tv);

        next_time = event_get_next_timeout(&tv);
        event_process_events(next_time);
    }
}

static void event_clear_timeouts(void)
{
    struct event_timeout *t,*tmp;

    list_for_each_entry_safe(t,tmp,&timeout_head,list) {

        event_timeout_cancel(t);
    }
}



/**
 * @brief 销毁事件循环
 */
void event_loop_done(void)
{
    event_cancelled = 1;

    if(poll_fd >= 0)
    {
        close(poll_fd);
        poll_fd = -1;
    }

    event_clear_timeouts();

}


void event_loop_cancel()
{
    event_cancelled = 1;
}
