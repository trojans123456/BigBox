#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "thread_async.h"

#define MAX_EVENTS 1024

static thread_async_queue_t *async_eventfd_queue_create(int size);
static int async_eventfd_queue_push_tail(thread_async_queue_t* q, queue_task_t* data);
static queue_task_t* async_eventfd_queue_pop_head(thread_async_queue_t* q, int timeout);
static void async_eventfd_queue_free(thread_async_queue_t* q);
static int async_eventfd_queue_empty(thread_async_queue_t* q);
static int async_eventfd_queue_destory(thread_async_queue_t* q);


const thread_async_queue_op_t async_eventfd_op =
{
    "eventfd",
    async_eventfd_queue_create,
    async_eventfd_queue_push_tail,
    async_eventfd_queue_pop_head,
    async_eventfd_queue_free,
    async_eventfd_queue_empty,
    async_eventfd_queue_destory
};

const thread_async_queue_op_t *get_ops()
{
    return &async_eventfd_op;
}

thread_async_queue_t *async_eventfd_queue_create(int size)
{
    thread_async_queue_t* q = (thread_async_queue_t*)malloc(sizeof (thread_async_queue_t));

    q->queue   = create_thread_queue(size);
    q->epollfd = epoll_create(MAX_EVENTS);
    q->tasked  = 0;
    if (q->epollfd == -1)
    {
        return NULL;
    }

    return q;
}

int async_eventfd_queue_push_tail(thread_async_queue_t* q, queue_task_t *task)
{
    unsigned long long i = 0xffffffff;
    if (!thread_queue_is_full(q->queue))
    {
        thread_queue_push_tail(q->queue, task);

        struct epoll_event ev;
        int efd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
        if (efd == -1) printf("eventfd create: %s", strerror(errno));
        ev.events = EPOLLIN ;// | EPOLLLT;
        ev.data.fd = efd;
        if (epoll_ctl(q->epollfd, EPOLL_CTL_ADD, efd, &ev) == -1)
        {
            return 0;
        }

        write(efd, &i, sizeof (i));

        return 1;
    }

    return 0;
}

queue_task_t* async_eventfd_queue_pop_head(thread_async_queue_t* q, int timeout)
{
    unsigned long long i = 0;
    struct epoll_event events[MAX_EVENTS];
    int nfds = epoll_wait(q->epollfd, events, MAX_EVENTS, -1);
    if (nfds == -1)
    {
        return NULL;
    }
    else
    {
        read(events[0].data.fd, &i, sizeof (i));
        close(events[0].data.fd); // NOTE: need to close here
        queue_task_t* task = thread_queue_pop_head(q->queue);

        if (task)
        {
            q->tasked ++;
            static long long precision = 10;
            if ((q->tasked % precision ) == 0)
            {
                precision *= 10;
            }
        }
        return task;
    }

    return NULL;
}

void async_eventfd_queue_free(thread_async_queue_t *q)
{
    thread_queue_free(q->queue);
    close(q->efd);
    free(q);
}

int async_eventfd_queue_empty(thread_async_queue_t* q)
{
    return thread_queue_is_empty(q->queue);
}

int async_eventfd_queue_destory(thread_async_queue_t* q)
{
    return 1;
}
