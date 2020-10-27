#include <stdio.h>

#include "lapi.h"

void *task1(hThread th,void *args)
{
    hCond cond = (hCond)args;
    if(!cond)
    {
        printf("cond is empty\n");
        return NULL;
    }

    while(1)
    {
        if(! lapi_thread_isrunning(th))
        {
            printf("break....1\n");
            break;
        }
        lapi_cond_lock(cond);
        lapi_cond_signal(cond);
        lapi_cond_unlock(cond);

        #if 0
        lapi_cond_lock(cond);
        ....
        lapi_cond_unlock(cond);
        lapi_cond_signal(cond);
        #endif // 0

        lapi_sleep(1000);
    }
}

void *task2(hThread th,void *args)
{
    hCond cond = (hCond)args;
    if(!cond)
        return NULL;
    int cnt = 1;

    while(1)
    {
        if(! lapi_thread_isrunning(th))
        {
            printf("break....2\n");
            break;
        }
        lapi_cond_lock(cond);
        lapi_cond_wait(cond);
        printf("########%d \n",cnt++);
        lapi_cond_unlock(cond);
    }
}

void cond_test()
{
    hCond cond = lapi_cond_create();
    if(!cond)
    {
        printf("cond create failed\n");
        return ;
    }

    hThread t1 = lapi_thread_create(task1,cond,0);
    if(!t1)
    {
        printf("create thread failed\n");
        return ;
    }
    hThread t2 = lapi_thread_create(task2,cond,0);
    if(!t2)
    {
        printf("create thread 2 failed\n");
        return ;
    }
}

void * mtx_task1(hThread th,void *args)
{
    hMutex mtx = (hMutex)args;
    if(!mtx)
        return NULL;
    int cnt = 1;

    while(1)
    {
        if(! lapi_thread_isrunning(th))
        {
            printf("break....1\n");
            break;
        }
        lapi_mutex_lock(mtx);
        printf("11111\n");
        //lapi_mutex_unlock(mtx);
        lapi_sleep(1000);
    }
}

void *mtx_task2(hThread th,void *args)
{
hMutex mtx = (hMutex)args;
    if(!mtx)
        return NULL;
    int cnt = 1;

    while(1)
    {
        if(! lapi_thread_isrunning(th))
        {
            printf("break....2\n");
            break;
        }
        lapi_mutex_lock(mtx);
        printf("22222\n");
        lapi_mutex_unlock(mtx);
        lapi_sleep(2000);
    }
}

void mutex_test()
{
    hMutex mtx = lapi_mutex_create();
    if(!mtx)
    {
        printf("mtx create failed\n");
        return ;
    }

    hThread t1 = lapi_thread_create(mtx_task1,mtx,0);
    if(!t1)
    {
        printf("create thread failed\n");
        return ;
    }
    hThread t2 = lapi_thread_create(mtx_task2,mtx,0);
    if(!t2)
    {
        printf("create thread 2 failed\n");
        return ;
    }
}

int main(int argc,char *argv[])
{
    //cond_test();
    mutex_test();
    getchar();

}
