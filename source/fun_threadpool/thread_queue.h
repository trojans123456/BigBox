#ifndef __THREAD_QUEUE_H
#define __THREAD_QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    void *(*run)(void *);
    void *args;
}queue_task_t;

typedef struct
{
    int head;
    int tail;
    int size;
    int capcity;
    queue_task_t *tasks;
}thread_queue_t;

//size task count
thread_queue_t *create_thread_queue(int task_cnt);
void delete_thread_queue(thread_queue_t *q);
int thread_queue_is_full(thread_queue_t *q);
int thread_queue_is_empty(thread_queue_t *q);
int thread_queue_push_tail(thread_queue_t *q,queue_task_t *data);
queue_task_t *thread_queue_pop_head(thread_queue_t *q);
void thread_queue_free(thread_queue_t *q);

#ifdef __cplusplus
}
#endif

#endif
