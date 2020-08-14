#include <stdlib.h>
#include <stdio.h>

#include "threadpoll.h"

static void *threadpool_thread(void *threadpool)
{
    threadpool_t *pool = (threadpool_t *)threadpool;
    if(!pool)
        return NULL;

    for(;;)
    {
        queue_task_t *task = pool->qop->pop(pool->queue,50);
        if(task)
        {
            printf("task = %d\n",atoi(task->args));
            task->run(task->args);
        }
        if(((pool->shutdown == immediate_shutdown) || (pool->shutdown == graceful_shutdown) )
            && (pool->qop->empty(pool->queue)))
        {
            printf("--- thread %d is exit.\n", (int)pthread_self());
            break;
        }
    }

    return NULL;
}

threadpool_t *threadpool_create(int thread_count,int queue_size)
{
    if(thread_count <= 0 || queue_size <= 0)
    {
        return NULL;
    }

    threadpool_t *pool = (threadpool_t *)calloc(1,sizeof(threadpool_t));
    if(!pool)
    {
        return NULL;
    }

    pool->tsize = thread_count;
    pool->qop = get_ops();
    pool->queue = (pool->qop->create)(queue_size);
    pool->qsize = queue_size;
    pool->shutdown = 0;

    pool->threads = (pthread_t *)calloc(1,sizeof(pthread_t) * thread_count);

    if(!pool->threads || !pool->queue)
    {
        if(pool->threads)
            free(pool->threads);
        if(pool->queue)
            free(pool->queue);
        free(pool);
        return NULL;
    }

    //create worker thead
    int i = 0;
    for(i = 0;i < pool->tsize;i++)
    {
        if(pthread_create(&(pool->threads[i]),NULL,threadpool_thread,(void *)pool) != 0)
        {
            threadpool_destroy(pool,0);
            return NULL;
        }
    }

    pool->started = 1;

    return pool;
}

int threadpool_add(threadpool_t *pool,void *(*routeine)(void *),void *arg)
{
    int err = 0;
    if(!pool || !routeine)
        return threadpool_invalid;

    do
    {
        if(pool->shutdown)
        {
            err = threadpool_shutdown;
            break;
        }

        queue_task_t task;
        task.run = routeine;
        task.args = arg;

        if(!pool->qop->push(pool->queue,&task))
        {
            ;
        }
    }while(0);

    return err;
}

static int threadpool_free(threadpool_t *pool)
{
    if(!pool || pool->started > 0)
        return -1;

    if(pool->threads)
    {
        free(pool->threads);
        pool->qop->free(pool->queue);
    }
    free(pool);

    return 0;
}

int threadpool_destroy(threadpool_t *pool,int flags)
{
    int i,err = 0;
    if(!pool)
        return threadpool_invalid;

    do
    {
        if(pool->shutdown)
        {
            err = threadpool_shutdown;
            break;
        }

        pool->shutdown = (flags & threadpool_graceful) ? graceful_shutdown : immediate_shutdown;

        //destroy all worker threads
        pool->qop->destroy(pool->queue);

        //join all worker thead
        for(i = 0;i < pool->tsize;i++)
        {
            if(pthread_join(pool->threads[i],NULL) != 0)
            {
                err = threadpool_thread_failure;
            }
        }

    }while(0);

    if(!err)
    {
        threadpool_free(pool);
    }
    return err;
}
