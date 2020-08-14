
#include "timer_list.h"

static int tv_diff(struct timeval *t1,struct timeval *t2)
{
    return (t1->tv_sec - t2->tv_sec) * 1000 +
            (t1->tv_usec - t2->tv_usec) / 1000;
}

void event_gettime(struct timeval *tv)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC,&ts);
    tv->tv_sec = ts.tv_sec;
    tv->tv_usec = ts.tv_nsec / 1000;
}

int event_timeout_add(struct list_head *tm_head,struct event_timeout *ev_tm)
{
    struct event_timeout *tmp = NULL;
    struct list_head *h = tm_head;

    if(ev_tm->pending)
        return -1;

    list_for_each_entry(tmp,tm_head,list){

        if(tv_diff(&tmp->time,&ev_tm->time) > 0)
        {
            h = &tmp->list;
            break;
        }
    }

    list_add_tail(&ev_tm->list,h);
    ev_tm->pending = true;

    return 0;
}

int event_timeout_set(struct list_head *tm_head,struct event_timeout *ev_tm,int msecs)
{
    struct timeval *time = &ev_tm->time;

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

    return event_timeout_add(tm_head,ev_tm);
}

int event_timeout_cancel(struct event_timeout *ev_tm)
{
    if(!ev_tm->pending)
        return -1;

    list_del(&ev_tm->list);
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

int event_get_next_timeout(struct list_head *tm_head,struct timeval *tv)
{
    struct event_timeout *timeout;
    int diff;

    if(!tm_head)
        return -1;

    if(list_empty(tm_head))
        return -1;

    timeout = list_first_entry(tm_head,struct event_timeout,list);
    diff = tv_diff(&timeout->time,tv);
    if(diff < 0)
        return 0;

    return diff;
}

void event_process_timeouts(struct list_head *tm_head, struct timeval *tv)
{
    struct event_timeout *t;
    if(!tm_head)
        return ;

    while(!list_empty(tm_head))
    {
        t = list_first_entry(tm_head,struct event_timeout,list);
        if(tv_diff(&t->time,tv) > 0)
        {
            break;
        }

        event_timeout_cancel(t);
        if(t->cb)
        {
            t->cb(t);
        }

    }
}

/*
 *
 * while(1)
 * {
 *      struct timeval tv;
 *      event_gettime(&tv);
 *      event_process_timeouts(&tv);
 *      int next_time = event_get_next_timeout(&tv);
 *
 * }
 *
*/
