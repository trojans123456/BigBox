#include <pthread.h>
#include <signal.h>
#ifndef __WIN32__
#include <sys/prctl.h>
#include <sys/resource.h>
#endif
#include <unistd.h>
#include <sys/time.h>

#include <errno.h>




#include "resalloc.h"
#include "thread.h"

using namespace ns_core;

Thread::Thread():tid(0),threadStatus(THREAD_STATUS_NEW)
{

}

Thread::~Thread()
{
    if(tid > 0)
    {
        pthread_cancel(tid);
        pthread_join(tid,NULL);
    }
}

void *Thread::run0(void *pVoid)
{
    //允许销毁线程
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);

    Thread *p = (Thread *)pVoid;
    p->run1();
    return p;
}

void *Thread::run1()
{
    threadStatus = THREAD_STATUS_RUNNING;
    tid = pthread_self();

    run();

    threadStatus = THREAD_STATUS_EXIT;
    tid = 0;

    pthread_exit(NULL);
}

bool Thread::start()
{
    return pthread_create(&tid,NULL,run0,this) == 0;
}

pthread_t Thread::getThreadId()
{
    return tid;
}

void Thread::threadSleep(unsigned long ms)
{
    usleep(ms * 1000);
}

int Thread::getStatus()
{
    return threadStatus;
}

int Thread::cancle()
{
    return pthread_cancel(tid);
}

int Thread::kill()
{
    pthread_kill(tid,SIGKILL);
}

void Thread::join()
{
    if(tid > 0)
    {
        pthread_join(tid,NULL);
    }
}

void Thread::join(unsigned long ms)
{
    if(tid == 0)
        return ;

    if(ms == 0)
    {
        join();
    }
    else
    {
        unsigned long k = 0;
        while(threadStatus != THREAD_STATUS_EXIT && k <= ms)
        {
            usleep(100);
            k++;
        }
    }
}

void Thread::stop()
{
    pthread_exit(NULL);
}

int Thread::yield()
{
    return pthread_yield();
}

int Thread::thread_detach(pthread_t pid)
{
    return thread_detach(pid);
}

int Thread::thread_join(pthread_t pid, void **retVal)
{
    return pthread_join(pid,retVal);
}

int Thread::thread_cancel(pthread_t pid)
{
    return pthread_cancel(pid);
}
