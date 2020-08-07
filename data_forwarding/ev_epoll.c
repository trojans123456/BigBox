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
    if(*pefd == -1)
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

}

static int32_t epoll_mod(void *ptr,struct event *ev)
{

}

static int32_t epoll_del(void *ptr,struct event *ev)
{

}

static int32_t epoll_run(struct eventOpe *ope,void *ptr,struct event *ev)
{

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
