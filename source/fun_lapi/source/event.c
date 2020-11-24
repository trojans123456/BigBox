#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#ifdef __linux__
#include <time.h>
#include <sys/epoll.h>
#endif

#include "task.h"
#include "list.h"
#include "event.h"

struct epoll ;
typedef struct fd_event
{
    void *priv_data;
    fd_event_cb_ptr func;
    int fd;
    int flags;
    boolean registered;/*是否已经添加到epoll队列中*/
    uint32_t events; /* read write error */
    struct epoll *owner;
    //boolean ref; //如果该文件描述符被fork dup了epoll_fd close时不会自动关闭监听的fd
    //man epoll
}fd_event_t;

typedef struct timeout_event
{
    struct dl_list list;
    boolean pending;/*是否已经添加*/
    timeout_event_cb_ptr func;
    void *priv_data;
    struct epoll *owner;
    struct timeval time;/* 超时时间*/
}timeout_event_t;

typedef struct epoll
{
    int epoll_fd;
    boolean stopped;/*标志*/
    struct dl_list timeout_head;
    struct epoll_event *listen_events; /*监听到事件 */
    int listen_max;
    fd_event_t **queue;/*处理队列*/
    hMutex mutex;
}epoll_t;

hFdEvent fd_event_new(int fd,event_type_t type)
{
    fd_event_t *ev = NULL;
    if(fd < 0)
        return NULL;
    ev = (fd_event_t *)calloc(1,sizeof(fd_event_t));
    if(!ev)
        return NULL;

    ev->events = type;
    ev->fd = fd;
    ev->func = NULL;
    ev->priv_data = NULL;
    ev->registered = 0;
    ev->owner = NULL;

    ev->flags = type;

    int fl;
    fl = fcntl(ev->fd,F_GETFL,0);
    fl |= O_NONBLOCK;
    fcntl(ev->fd,F_SETFL,fl);

    return (hFdEvent)ev;
}

void fd_event_delete(hFdEvent fd)
{
    fd_event_t *ev = (fd_event_t *)fd;

    if(ev)
    {
       if(ev->registered && ev->owner)
           fd_event_del((hEpoll)ev->owner,fd);
       free(ev);
    }
}

int fd_event_setfunc(hFdEvent fd,fd_event_cb_ptr func,void *args)
{
    fd_event_t *ev = (fd_event_t *)fd;
    if(!ev)
        return -1;
    ev->func = func;
    ev->priv_data = args;

    return 0;
}


int fd_event_add(hEpoll e,hFdEvent fd)
{
    epoll_t *epoll = (epoll_t *)e;
    fd_event_t *ev = (fd_event_t *)fd;
    if(!epoll || !ev)
        return -1;

    if(ev->registered)
        return -1;

    ev->owner = epoll;
    struct epoll_event event;

    memset(&event,0,sizeof(struct epoll_event));

    if(ev->flags & EVENT_READ)
    {
        event.events |= (EPOLLIN |EPOLLRDHUP);
    }
    if(ev->flags & EVENT_WRITE)
    {
        event.events |= EPOLLOUT;
    }

    event.data.ptr = ev;

    lapi_mutex_lock(epoll->mutex);
    if(epoll_ctl(epoll->epoll_fd,EPOLL_CTL_ADD,ev->fd,&event) < 0)
    {
        lapi_mutex_unlock(epoll->mutex);
        return -1;
    }

    ev->registered = 1;
    lapi_mutex_unlock(epoll->mutex);

    return 0;
}

int fd_event_del(hEpoll e,hFdEvent fd)
{
    epoll_t *epoll = (epoll_t *)e;
    fd_event_t *ev = (fd_event_t *)fd;
    int ret = -1;

    if(!epoll || !ev)
        return -1;

    if(ev->registered == 0)
        return -1;

    lapi_mutex_lock(epoll->mutex);
    ret = epoll_ctl(epoll->epoll_fd,EPOLL_CTL_DEL,ev->fd,0);
    lapi_mutex_unlock(epoll->mutex);

    return ret;
}

int fd_event_getevents(hFdEvent fd)
{
    fd_event_t *ev = (fd_event_t *)fd;
    if(!ev)
        return -1;
    return ev->events;
}

/**** timeout event ****/
static int tv_diff(struct timeval *t1,struct timeval *t2)
{
    return (t1->tv_sec - t2->tv_sec) * 1000 +
                (t1->tv_usec - t2->tv_usec) / 1000;
}

static void gettime(struct timeval *tv)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC,&ts);
    tv->tv_sec = ts.tv_sec;
    tv->tv_usec = ts.tv_nsec / 1000;
}

hTmEvent timeout_event_new(int ms)
{
    timeout_event_t *tm = NULL;
    if(ms <= 0)
        return NULL;

    tm = (timeout_event_t *)calloc(1,sizeof(timeout_event_t));
    if(!tm)
        return NULL;

    tm->func = NULL;
    tm->pending = 0;
    dl_list_init(&tm->list);
    tm->priv_data = NULL;
    tm->owner = NULL;

    tm->time.tv_sec = (ms / 1000);
    tm->time.tv_usec = (ms % 1000) * 1000;

    return (hTmEvent)tm;
}

int timeout_event_delete(hTmEvent tt)
{
    timeout_event_t *tm = (timeout_event_t *)tt;
    if(!tm)
        return -1;
    if(tm->pending)
    {
        timeout_event_cancel(tt);
    }
    free(tm);
    return 0;
}

int timeout_event_setfunc(hTmEvent tt,timeout_event_cb_ptr func,void *args)
{
    timeout_event_t *tm = (timeout_event_t*)tt;
    if(!tm)
        return -1;
    if(tm->pending)
        return -1;

    tm->func = func;
    tm->priv_data = args;
    return 0;
}

int timeout_event_add(hEpoll e,hTmEvent tt)
{
    epoll_t *epoll = (epoll_t *)e;
    timeout_event_t *tm = (timeout_event_t*)tt;
    struct dl_list *h = NULL;
    timeout_event_t *item = NULL;

    if(!epoll || !tm)
        return -1;
    if(tm->pending)
        return -1;

    h = &epoll->timeout_head;
    tm->owner = epoll;

    dl_list_for_each(item,&epoll->timeout_head,timeout_event_t,list)
    {
        if(item)
        {
            if(tv_diff(&item->time,&tm->time) > 0)
            {
                h = &item->list;
                break;
            }
        }
    }

    dl_list_add_tail(h,&tm->list);
    tm->pending = 1;
    return 0;
}

int timeout_event_settime(hTmEvent tt,int ms)
{
    timeout_event_t *tm = (timeout_event_t *)tt;
    if(!tm)
        return -1;
    struct timeval *time = &tm->time;
    if(tm->pending)
    {
        timeout_event_cancel(tt);
    }
    gettime(time);

    time->tv_sec += ms / 1000;
    time->tv_usec += (ms % 1000) * 1000;

    if(time->tv_usec > 1000000)
    {
        time->tv_sec++;
        time->tv_usec -= 1000000;
    }

    return timeout_event_add((hEpoll)tm->owner,(hTmEvent)tm);
}

int timeout_event_cancel(hTmEvent tt)
{
    timeout_event_t *tm = (timeout_event_t *)tt;
    if(!tm)
        return -1;
    if(!tm->pending)
        return -1;
    dl_list_del(&tm->list);
    tm->pending = 0;

    return 0;
}

int timeout_event_remaining(hTmEvent tt)
{
    timeout_event_t *tm = (timeout_event_t *)tt;
    if(!tm)
        return -1;

    struct timeval now;
    if(!tm->pending)
        return -1;

    gettime(&now);

    return tv_diff(&tm->time,&now);
}

hEpoll event_loop_init(unsigned int max)
{
    epoll_t *hand = NULL;

    if(max == 0)
        return NULL;

    hand = (epoll_t *)calloc(1,sizeof(epoll_t));
    if(!hand)
        return NULL;

    hand->epoll_fd = epoll_create(max);
    if(hand->epoll_fd < 0)
    {
        free(hand);
        return NULL;
    }
    printf("111111\n");
    /* for fork */
    fcntl(hand->epoll_fd,F_SETFD,fcntl(hand->epoll_fd,F_GETFD) | FD_CLOEXEC);

    hand->stopped = 0;

    dl_list_init(&hand->timeout_head);

    hand->listen_events = (struct epoll_event *)calloc(max,sizeof(struct epoll_event));
    hand->listen_max = max;

    /*这里可以用goto 但因限制不能使用*/
    if(!hand->listen_events)
    {
        close(hand->epoll_fd);
        free(hand);
        return NULL;
    }
    printf("22222\n");
    hand->mutex = lapi_mutex_create();
    if(!hand->mutex)
    {
        free(hand->listen_events);
        close(hand->epoll_fd);
        free(hand);
        return NULL;
    }
    hand->queue = (fd_event_t **)calloc(max,sizeof(fd_event_t *));
    if(! hand->queue)
    {
        lapi_mutex_destroy(hand->mutex);
        free(hand->listen_events);
        close(hand->epoll_fd);
        free(hand);
        return NULL;
    }

    return (hEpoll)hand;
}

void event_loop_release(hEpoll e)
{
    epoll_t *epoll = (epoll_t *)e;
    if(epoll)
    {
        epoll->stopped = 1;

        /* clr timeout event */
        timeout_event_t *t,*tmp;
        dl_list_for_each_safe(t,tmp,&epoll->timeout_head,timeout_event_t,list)
        {
            timeout_event_cancel((hTmEvent)t);
        }

        close(epoll->epoll_fd);
        if(epoll->listen_events)
            free(epoll->listen_events);
        if(epoll->mutex)
            lapi_mutex_destroy(epoll->mutex);
        if(epoll->queue)
            free(*epoll->queue);


        free(epoll);
    }
}

void event_loop_cancel(hEpoll e)
{
    epoll_t *epoll = (epoll_t *)e;
    if(epoll)
    {
        lapi_mutex_lock(epoll->mutex);
        epoll->stopped = 1;
        lapi_mutex_unlock(epoll->mutex);
    }
}

void event_loop_run(hEpoll e)
{
    epoll_t *hand = (epoll_t *)e;
    struct timeval tv;
    int next_time = -1;
    timeout_event_t *timeout;
    struct epoll_event formod;
    /* */
    timeout_event_t *t;

    /* for fd process */
    int n,nfds;
    fd_event_t *cur;
    int q_index = 0;

    if(!hand)
        return ;

    while(!hand->stopped)
    {
        gettime(&tv);
        /** 处理 timeout事件*/
        while(!dl_list_empty(&hand->timeout_head))
        {
            t = dl_list_first(&hand->timeout_head,timeout_event_t,list);
            if(tv_diff(&t->time,&tv) > 0)
                break;

            timeout_event_cancel((hTmEvent)t);
            if(t->func)
            {
                t->func((hTmEvent)t,t->priv_data);
            }
        }
        printf("stop = %d\n",hand->stopped);
        if(hand->stopped)
            break;

        gettime(&tv);
        next_time = -1;

        if(!dl_list_empty(&hand->timeout_head))
        {
           timeout = dl_list_first(&hand->timeout_head,timeout_event_t,list);
           next_time = tv_diff(&timeout->time,&tv);
           if(next_time < 0)
           {
               next_time = 0;
           }
        }

        /** 获取 fd事件*/
        {
            nfds = epoll_wait(hand->epoll_fd,hand->listen_events,hand->listen_max,next_time);
            if(nfds <= 0)
                continue;

            for(n = 0;n < nfds;n++)
            {
                cur = hand->listen_events[n].data.ptr;
                if(!cur)
                    continue;

                hand->queue[q_index] = cur;
                q_index++;


                if(hand->listen_events[n].events & (EPOLLERR | EPOLLHUP))
                {
                    /* error */
                    fd_event_del(e,(hFdEvent)cur);
                    cur->events |= EVENT_ERROR;
                }

                /* 对端关闭*/
                if(hand->listen_events[n].events & (EPOLLRDHUP))
                {

                }
                if(hand->listen_events[n].events & (EPOLLIN))
                {
                    cur->events |= EVENT_READ;

                    /* modify */
                    memset(&formod,0,sizeof(struct epoll_event));
                    formod.events |= EPOLLOUT;
                    formod.data.ptr = cur;
                    epoll_ctl(hand->epoll_fd,EPOLL_CTL_MOD,cur->fd,&formod);
                }

                if(hand->listen_events[n].events & (EPOLLOUT))
                {
                    cur->events |= EVENT_WRITE;
                    /* modify */
                    memset(&formod,0,sizeof(struct epoll_event));
                    formod.events |= EPOLLIN;
                    formod.data.ptr = cur;
                    epoll_ctl(hand->epoll_fd,EPOLL_CTL_MOD,cur->fd,&formod);
                }
            }
        }
        /** 处理 fd事件*/
        {
            q_index = 0;

            while(nfds > 0)
            {
                cur = hand->queue[q_index];
                nfds--;

                if(cur && cur->func)
                {
                    cur->func((hFdEvent)cur,cur->priv_data);
                }
            }
        }
    }
}
