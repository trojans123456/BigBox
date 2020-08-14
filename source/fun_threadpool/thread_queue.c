#include <stdlib.h>

#include "thread_queue.h"

#define DEFAULT_QUEUE_SIZE  65535


thread_queue_t *create_thread_queue(int size)
{
    thread_queue_t *que = (thread_queue_t *)calloc(1,sizeof(thread_queue_t));
    if(!que)
    {
        return NULL;
    }

    if(size > 0)
    {
        que->tasks = (queue_task_t *)calloc(1,sizeof(queue_task_t) * size);
        que->capcity = size;
    }
    else
    {
        int default_size = DEFAULT_QUEUE_SIZE;
        que->tasks = (queue_task_t *)calloc(1,sizeof(queue_task_t) * default_size);
        que->capcity = default_size;
    }

    if(!que->tasks)
    {
        free(que);
        return NULL;
    }
    que->head = que->tail = que->size = 0;

    return que;
}
void delete_thread_queue(thread_queue_t *q)
{
    if(q)
    {
        if(q->tasks)
            free(q->tasks);
        free(q);
    }
}
int thread_queue_is_full(thread_queue_t *q)
{
    if(q)
    {
        return (q->size == q->capcity);
    }
    return 0;
}
int thread_queue_is_empty(thread_queue_t *q)
{
    if(q)
    {
        return (q->size == 0);
    }
    return 0;
}
int thread_queue_push_tail(thread_queue_t *q,queue_task_t *data)
{
    if(q)
    {
        if(!thread_queue_is_full(q))
        {
            q->tasks[q->tail].run = data->run;
            q->tasks[q->tail].args = data->args;
            q->tail = (q->tail + 1) % q->capcity;
            q->size++;
            return 1;
        }
    }
    return 0;
}
queue_task_t *thread_queue_pop_head(thread_queue_t *q)
{
    if(q)
    {
        queue_task_t *t = NULL;
        if(!thread_queue_is_empty(q))
        {
            t = &q->tasks[(q->head)];
            q->head = (q->head + 1) % q->capcity;
            q->size--;
        }
        return t;
    }
    return NULL;
}

void thread_queue_free(thread_queue_t *q)
{
    free(q->tasks);
    free(q);
}
