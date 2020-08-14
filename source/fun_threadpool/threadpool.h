#ifndef __THREAD_POOL_H
#define __THREAD_POOL_H

#include "thread_async.h"

#ifdef __cplusplus
extern "C" {
#endif



typedef struct
{
    pthread_t *threads;//thread count
    const thread_async_queue_op_t *qop;
    thread_async_queue_t *queue;
    int qsize;
    int tsize;//threads count
    int shutdown;
    int started;
}threadpool_t;

typedef enum
{
    threadpool_invalid        = -1,
    threadpool_lock_failure   = -2,
    threadpool_queue_full     = -3,
    threadpool_shutdown       = -4,
    threadpool_thread_failure = -5,
    threadpool_memory_error   = -6
}threadpool_error_t;

typedef enum
{
    immediate_shutdown = 1,
    graceful_shutdown  = 2
}threadpool_shutdown_t;

typedef enum
{
    threadpool_graceful       = 1
}threadpool_destroy_flags_t;

threadpool_t *threadpool_create(int thread_count,int queue_size);

//argsÐèÒªÍâ²¿malloc
int threadpool_add(threadpool_t *pool,void *(*routeine)(void *),void *arg);

int threadpool_destroy(threadpool_t *pool,int flags);

#ifdef __cplusplus
}
#endif

#endif
