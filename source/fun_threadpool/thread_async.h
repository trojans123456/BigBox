#ifndef E_THREAD_ASYNC_H_
#define E_THREAD_ASYNC_H_

#include <pthread.h>
#include "thread_queue.h"

#ifdef __cplusplus
extern "C" {
#endif



typedef struct
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int waiting_threads;
    thread_queue_t *queue;
    int quit;//0 exit 1 not exit
    int efd;//event fd
    int epollfd;//epoll fd
    long long tasked;//complete task cnt
}thread_async_queue_t;

typedef struct
{
    const char *name;//such as cond eventfd lock free
    thread_async_queue_t* (*create)(int size);
    int (*push)(thread_async_queue_t *queue,queue_task_t *t);
    queue_task_t* (*pop)(thread_async_queue_t *queue,int timeout);
    void (*free)(thread_async_queue_t *queue);
    int (*empty)(thread_async_queue_t *queue);
    int (*destroy)(thread_async_queue_t *queue);
}thread_async_queue_op_t;//thread ops

const thread_async_queue_op_t *get_ops();

#ifdef __cplusplus
}
#endif

#endif // E_THREAD_ASYNC_H_
