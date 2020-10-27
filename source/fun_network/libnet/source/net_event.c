#ifdef __linux__
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "net_event.h"


#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr)     (sizeof(arr) / sizeof((arr)[0]))
#endif

static int tv_diff(struct timeval *t1,struct timeval *t2)
{
    return (t1->tv_sec - t2->tv_sec) * 1000 +
            (t1->tv_usec - t2->tv_usec) / 1000;
}

static void event_gettime(struct timeval *tv)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC,&ts);
    tv->tv_sec = ts.tv_sec;
    tv->tv_usec = ts.tv_nsec / 1000;
}

static int register_poll(int poll_fd,struct event_fd *fd,uint32_t flags)
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

static int unregister_poll(int poll_fd,struct event_fd *fd)
{
    return epoll_ctl(poll_fd,EPOLL_CTL_DEL,fd->fd,0);
}

int event_fd_add(struct event_reactor *reactor, struct event_fd *ev_fd, uint8_t flag)
{
    uint32_t fl;
    int ret;

    if(!reactor || !ev_fd)
        return -1;

    if(!(flag & (EVENT_READ | EVENT_WRITE)))
    {
        return event_fd_delete(reactor,ev_fd);
    }

    if(!ev_fd->registered && !(flag & EVENT_BLOCKING))
    {
        printf("##########\n");
        fl = fcntl(ev_fd->fd,F_GETFL,0);
        fl |= O_NONBLOCK;
        fcntl(ev_fd->fd,F_SETFL,fl);
    }

    ret = register_poll(reactor->listen_fd,ev_fd,flag);
    if(ret < 0)
    {
        return ret;
    }

    ev_fd->registered = true;
    ev_fd->eof = false;
    ev_fd->error = false;

    return ret;
}

int event_fd_delete(struct event_reactor *reactor, struct event_fd *ev_fd)
{
    if(!reactor || !ev_fd)
        return -1;

    if(!ev_fd->registered)
        return -1;

    ev_fd->registered = false;

    return unregister_poll(reactor->listen_fd,ev_fd);
}

void event_clear_fds(struct event_reactor *reactor)
{
    if(reactor)
    {
        int i;
        for(i = 0;i < ARRAY_SIZE(reactor->cur_fds);i++)
        {
            if(reactor->cur_fds[i].fd > 0)
            {
                if(event_fd_delete(reactor,(reactor->cur_fds[i].fd)) > 0)
                {
                    close(reactor->cur_fds[i].fd->fd);
                    reactor->cur_fds[i].fd->fd = -1;
                }
            }
        }

    }
}

static int event_init_pollfd(uint32_t max_fd)
{
    int poll_fd = -1;
    if(max_fd == 0)
        return -1;

    poll_fd = epoll_create(max_fd);
    if(poll_fd < 0)
        return -1;

    fcntl(poll_fd,F_SETFD,fcntl(poll_fd,F_GETFD) | FD_CLOEXEC);

    return poll_fd;
}

static void fd_free(struct event_fd *fd)
{
    if(fd)
    {
        free(fd);
    }
}

struct event_reactor *event_loop_new(uint32_t max_fd)
{
    struct event_reactor *a_reactor = NULL;

    a_reactor = (struct event_reactor *)calloc(1,sizeof(struct event_reactor));
    if(!a_reactor)
        return NULL;

    a_reactor->max_fd = max_fd;
    if((a_reactor->listen_fd = event_init_pollfd(a_reactor->max_fd)) < 0)
    {
        free(a_reactor);
        return NULL;
    }

    a_reactor->h_free = fd_free;
    a_reactor->event_canclled = false;
    init_list_head(&(a_reactor->timeout_head));

    return a_reactor;
}

void event_loop_delete(struct event_reactor *reactor)
{
    if(reactor)
    {
        reactor->event_canclled = true;
        if(reactor->listen_fd >= 0)
        {
            event_clear_fds(reactor);
            event_clear_timeouts(reactor);

            close(reactor->listen_fd);
            reactor->listen_fd = -1;
        }

    }
}

static void event_process_timeouts(struct event_reactor *reactor,struct timeval *tv)
{
    struct event_timeout *t;

    while(!list_empty(&reactor->timeout_head))
    {
        t = list_first_entry(&reactor->timeout_head,struct event_timeout,head);
        if(tv_diff(&t->time,tv) > 0)
        {
            break;
        }

        event_timeout_cancel(t);
        if(t->h_timeout)
        {
            t->h_timeout(reactor,t);
        }
    }
}


static int event_get_next_timeout(struct event_reactor *reactor,struct timeval *tv)
{
    struct event_timeout *timeout;
    int diff;

    if(list_empty(&reactor->timeout_head))
        return -1;

    timeout = list_first_entry(&reactor->timeout_head,struct event_timeout,head);
    diff = tv_diff(&timeout->time,tv);
    if(diff < 0)
        return 0;

    return diff;
}

static int event_fetch_events(struct event_reactor *reactor,int timeout)
{
    int n, nfds;
    struct epoll_event events[MAX_FD];
    struct event_fd *u = NULL;

    struct event_fd_event *cur = NULL;

    nfds = epoll_wait(reactor->listen_fd, events, ARRAY_SIZE(events), timeout);
    if(nfds == -1)
    {
        return -1;
    }
    if(nfds == 0)
    {
        /* timeout */
        printf("timeout....\n");
        return 0;
    }

    for (n = 0; n < nfds; ++n) {

        cur = &(reactor->cur_fds[n]);
        u = events[n].data.ptr;
        unsigned int ev = 0;

        cur->fd = u;
        //printf("cur_fd = %d\n",cur->fd ? cur->fd->fd : 999);
        if (!u)
            continue;

        if (events[n].events & (EPOLLERR|EPOLLHUP)) {
            u->error = true;
            //if (!(u->flags & EVENT_ERROR))
            event_fd_delete(reactor,u);
            if(reactor->h_free)
            {
                printf("free......\n");
                reactor->h_free(u);
            }
        }

        if(!(events[n].events & (EPOLLRDHUP|EPOLLIN|EPOLLOUT|EPOLLERR|EPOLLHUP))) {
            cur->fd = NULL;
            continue;
        }
        /*对端关闭 */
        if(events[n].events & EPOLLRDHUP)
            u->eof = true;

        if(events[n].events & EPOLLIN)
        {
            ev |= EVENT_READ;

            /* Action!!!! */
            events[n].events = EPOLLOUT;
            epoll_ctl(reactor->listen_fd,EPOLL_CTL_MOD,cur->fd->fd,&events[n]);
        }


        if(events[n].events & EPOLLOUT)
        {
            ev |= EVENT_WRITE;

            /* Action!!!!! */
            events[n].events = EPOLLIN;
            epoll_ctl(reactor->listen_fd,EPOLL_CTL_MOD,cur->fd->fd,&events[n]);
        }


        cur->events = ev;

    }

    return nfds;
}

static void event_process_events(struct event_reactor *reactor,int timeout)
{
    struct event_fd_event *cur;
    struct event_fd *fd;
    unsigned int events;
    static int cur_fd = 0,cur_nfds = 0;


    cur_nfds = event_fetch_events(reactor,timeout);

    if(cur_nfds > 0)
    {
        int i = 0;
        for(i = 0;i < cur_nfds;i++)
        {
            cur = &(reactor->cur_fds[i]);
            fd = cur->fd;
            events = cur->events;
            if(!fd)
                continue;
            if(!fd->h_fd)
                continue;
            //printf("complete h_fd....\n");
            fd->h_fd(reactor,fd,events);
            memset(&(reactor->cur_fds[i]),0,sizeof(reactor->cur_fds[i]));
        }

    }
}

void event_loop_run(struct event_reactor *reactor)
{
    int next_time = 0;
    struct timeval tv;
    if(!reactor)
        return ;

    while(!reactor->event_canclled)
    {
        event_gettime(&tv);
        event_process_timeouts(reactor,&tv);

        if(reactor->event_canclled)
            break;

        event_gettime(&tv);

        next_time = event_get_next_timeout(reactor,&tv);
        event_process_events(reactor,next_time);
    }
}

void event_loop_cancel(struct event_reactor *reactor)
{
    if(reactor)
        reactor->event_canclled = true;
}


int event_timeout_add(struct event_reactor *reactor, struct event_timeout *ev_tm)
{
    struct event_timeout *tmp = NULL;
    struct list_head *h = NULL;

    if(!reactor || !ev_tm)
        return -1;
    if(ev_tm->pending)
        return -1;

    h = &reactor->timeout_head;

    list_for_each_entry(tmp,&reactor->timeout_head,head) {
        if(tv_diff(&tmp->time,&ev_tm->time) > 0)
        {
            h = &tmp->head;
            break;
        }
    }

    list_add_tail(&ev_tm->head,h);
    ev_tm->pending = true;

    return 0;
}



int event_timeout_set(struct event_reactor *reactor, struct event_timeout *ev_tm, int msecs)
{
    struct timeval *time = &ev_tm->time;

    if(!reactor || !ev_tm)
        return -1;

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

    return event_timeout_add(reactor,ev_tm);
}


int event_timeout_cancel(struct event_timeout *ev_tm)
{
    if(!ev_tm->pending)
        return -1;

    list_del(&ev_tm->head);
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

void event_clear_timeouts(struct event_reactor *reactor)
{
    struct event_timeout *t,*tmp;
    if(!reactor)
        return ;

    list_for_each_entry_safe(t,tmp,&reactor->timeout_head,head){

        event_timeout_cancel(t);
    }
}
