#include <stdio.h>
#include <stdlib.h>

#include "task.h"
#include "ioengine.h"

typedef struct
{
    hMutex mtx;
    int loop;
    ioe_params par;
    IOS ios;
    hThread th;
}TIOE;

static void *task_run(hThread th,void *priv)
{
    TIOE *ioe = (TIOE *)priv;
    if(!ioe)
        return NULL;
    int ev = 0;
    int i = 0;

    while(lapi_thread_isrunning(th))
    {
        ev = ioe->ios.ioswitch(&ioe->ios,-1);
        if(ev <= 0)
        {
            lapi_sleep(10);
            continue;
        }

        for(i = 0;i < ev;i++)
        {
            hSock s = ioe->ios.evarr[i];
            if(ioe->par.cb)
            {
                ioe->par.cb((hIOEngine)ioe,s,ioe->par.ctx);
            }
        }
    }
    return NULL;
}

hIOEngine ioengine_startup(ioe_params *param)
{
    TIOE *ioe;

    if(!param)
        return NULL;

    ioe = (TIOE*)calloc(1,sizeof(TIOE));
    if(!ioe)
        return NULL;

    ioe->mtx = lapi_mutex_create();
    ioe->loop = 0;
    ioe->par = *param;

    ios_epoll_init(&ioe->ios,ioe->par.listen_max);

    ioe->th = lapi_thread_create(task_run,ioe,512 <<10);
    if(!ioe->th)
    {
        lapi_mutex_destroy(ioe->mtx);
        free(ioe);
        return NULL;
    }

    return (hIOEngine)ioe;
}

void ioengine_cleanup(hIOEngine h)
{
    TIOE *ioe = (TIOE*)h;
    if(ioe)
    {
        lapi_thread_destroy(ioe->th);
        ioe->ios.destroy(&ioe->ios);
        lapi_mutex_destroy(ioe->mtx);

        free(ioe);
    }
}

int ioengine_add(hIOEngine h, hSock sock)
{
    TIOE *ioe = (TIOE*)h;
    int ret;

    if(!ioe || !sock)
        return -1;

    lapi_mutex_lock(ioe->mtx);
    ret = ioe->ios.add(&ioe->ios,sock);
    lapi_mutex_unlock(ioe->mtx);
    return ret;
}

int ioengine_del(hIOEngine h, hSock sock)
{
    TIOE *ioe = (TIOE *)h;
    if(!ioe || !sock)
        return -1;

    lapi_mutex_lock(ioe->mtx);
    int ret = ioe->ios.del(&ioe->ios,sock);
    lapi_mutex_unlock(ioe->mtx);

    return ret;
}
