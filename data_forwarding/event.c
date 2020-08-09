
#include "event.h"

int32_t eventListInit(struct event_list *a_list)
{
    if(a_list == NULL)
    {
        return -1;
    }

    INIT_LIST_HEAD(&a_list->ev_list);
    a_list->nodeNum = 0;

    return 0;
}
int32_t eventListAdd(struct event_list *a_head,struct event *a_ev)
{
    if(a_head == NULL || a_ev == NULL)
        return -1;

    list_add_tail(&a_ev->head,&a_head->ev_list);
    a_head->nodeNum += 1;

    return 0;
}
int32_t eventListDel(struct event_list *a_head,struct event *a_ev)
{
    if(a_head == NULL || a_ev == NULL)
        return -1;

    list_del(&a_ev->head);
    a_head->nodeNum -= 1;

    return 0;
}

extern struct event_ops evops_epoll;
struct reactor *reactorInit(int32_t maxConNum)
{
    struct reactor *a_this = (struct reactor *)calloc(1,sizeof(struct reactor));
    if(!a_this)
        return NULL;

    eventListInit(&a_this->activeQueue);
    eventListInit(&a_this->timeoutQueue);

    a_this->sysOps = &evops_epoll;
    a_this->maxConNum = maxConNum;

    a_this->evOper.activeQueue = &a_this->activeQueue;
    a_this->evOper.evAdd = &eventListAdd;

    /* init epoll */
    a_this->listen_fd = a_this->sysOps->init();
    if(!a_this->listen_fd)
    {
        free(a_this);
        return NULL;
    }

    a_this->minheap = minHeapInit(maxConNum);
    if(!a_this->minheap)
    {
        a_this->sysOps->destroy(a_this->listen_fd);
        free(a_this);
        return NULL;
    }

    return a_this;
}
void reactorDestroy(struct reactor *a_this)
{
    if(a_this)
    {
        a_this->sysOps->destroy(a_this->listen_fd);
        a_this->minheap->des(a_this->minheap);
        free(a_this);
    }

}


int reactorRun(struct reactor *a_this);

int eventInit(struct event *ev,int fd,int mode,eventExecCb ev_exe,void *args)
{
    if(ev == NULL)
        return -1;

    ev->fd = fd;
    ev->mode = mode;
    ev->ev_mode = 0;
    ev->evExec = ev_exe;
    ev->args = args;
    INIT_LIST_HEAD(&ev->head);
    ev->minHeapFlag = OUT_MINHEAP;
    ev->timeDelay = 0;

    return 0;
}

int eventAddListen(struct reactor *a_this,struct event *ev)
{
    eventListAdd(&a_this->activeQueue,ev);
    a_this->sysOps->add(a_this->listen_fd,ev);

    return 0;
}
int eventRmListen(struct reactor *a_this,struct event *ev)
{
    a_this->sysOps->del(a_this->listen_fd,ev);

    eventListDel(&a_this->activeQueue,ev);
    return 0;
}
int eventModListen(struct reactor *a_this,struct event *ev,int32_t mode)
{
    eventRmListen(a_this,ev);

    eventInit(ev,ev->fd,mode,ev->evExec,ev->args);

    eventAddListen(a_this,ev);

    return 0;
}

static int32_t getTimeout(struct reactor *a_this)
{
    if(a_this->minheap->headIndex != a_this->minheap->lastIndex)
    {
        return a_this->minheap->nodeList[a_this->minheap->headIndex].key;
    }
    return -1;
}

int reactorRun(struct reactor *a_this)
{
    if(a_this == NULL)
        return -1;
    int ret = 0;
    struct list_head *cur_list = NULL;
    struct event *cur_event = NULL;

    while(1)
    {
        ret = a_this->sysOps->run(&a_this->sysOps,getTimeout(a_this));
        if(ret != -1)
        {
            list_for_each(cur_list,&a_this->activeQueue)
            {
                cur_event = container_of(cur_list,struct event,head);

                cur_event->evExec(cur_event->args);
            }
        }
    }
}
