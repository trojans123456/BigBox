#include <stdlib.h>

#ifndef __WIN32
    #include <sys/epoll.h>
#endif

#include "event.h"

#define MAX_FD  512

static void *epoll_init(void)
{
    int *epfd = NULL;
    epfd = calloc(1,sizeof(int));
    if(!epfd)
    {
        return NULL;
    }

    *epfd = epoll_create(MAX_FD);
    if(*epfd == -1)
    {
        free(*epfd);
        return NULL;
    }

    return (void *)epfd;
}

static void epoll_destroy(void *ptr)
{
    int *epfd = (int *)ptr;
    if(epfd)
    {
        close(*epfd);
        free(ptr);
    }
}

static int32_t epoll_add(void *ptr,struct event *ev)
{
    struct epoll_event epev = {0};
    epev.events = 0;
    int *epfd = (int *)ptr;
    if(!epfd)
    {
        return -1;
    }

    if(ev->mode & EVENT_READ)
    {
        epev.events |= EPOLLIN;
    }
    if(ev->mode & EVENT_WRITE)
    {
        epev.events |= EPOLLOUT;
    }

    epev.data.fd = ev->fd;
    epev.data.ptr = ev;

    if(epoll_ctl(*epfd,EPOLL_CTL_ADD,ev->fd,&epev) < 0)
    {
        return -1;
    }
    return 0;
}

static int32_t epoll_mod(void *ptr,struct event *ev)
{
    struct epoll_event epev = {0};
    epev.events = 0;
    int *epfd = (int *)ptr;
    if(!epfd)
    {
        return -1;
    }

    if(!ev->fd) {
        return -1;
    }
    if(ev->mode & EVENT_READ) {
        epev.events |= EPOLLIN;
    }
    if(ev->mode & EVENT_WRITE) {
        epev.events |= EPOLLOUT;
    }
    if(!(ev->mode & EVENT_READ)) {
        epev.events &= (~EPOLLIN);
    }
    if(!(ev->mode & EVENT_WRITE)) {
        epev.events &= (~EPOLLOUT);
    }

    epev.data.fd = ev->fd;
    epev.data.ptr = ev;
    if(epoll_ctl(*epfd, EPOLL_CTL_MOD, ev->fd, &epev) < 0) {
        return -1;
    }
    return 0;
}

static int32_t epoll_del(void *ptr,struct event *ev)
{
    int *epfd = (int *)ptr;
    if(epfd)
    {
        epoll_ctl(*epfd,EPOLL_CTL_DEL,ev->fd,0);
        return 0;
    }
    return -1;
}

static int32_t epoll_run(struct eventOpe *ope,void *ptr,int32_t timeout)
{
    int result = 0;
    int pos = 0;
    struct epoll_event  epev[MAX_FD];
    struct event *ev;
    int *epfd = (int *)ptr;
    if(!epfd)
    {
        return -1;
    }

    result = epoll_wait(*epfd,epev,MAX_FD,timeout);
    if(result == -1)
    {
        goto out;
    }
    if(err == 0)
    {
        /* timeout */
        goto out;
    }


    for(pos = 0;pos < result;pos++)
    {
        ev = epev[i].data.ptr;
        if(epev[i].events & EPOLLIN)
        {
            /* read */
            if(epev[i].events == (EPOLLIN | EPOLLERR | EPOLLHUP))
            {
                ev->ev_mode |= EVENT_ERROR;
            }
            else
            {
                ev->ev_mode |= EVENT_READ;
            }
        }
        if(epev[i].events & EPOLLOUT)
        {
            ev->ev_mode |= EVENT_WRITE;
        }

        ope->evAdd(&ope->activeQueue->ev_list,ev);
    }

    return 0;
out:
    return result;
}


struct event_ops evops_epoll =
{
    "Linux_Epoll",
    &epoll_init,
    &epoll_destroy,
    &epoll_add,
    &epoll_mod,
    &epoll_del,
    &epoll_run
};
