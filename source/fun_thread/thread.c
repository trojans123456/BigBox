<<<<<<< HEAD
#include <stdio.h>
#include <signal.h>
#include <sched.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include <sys/prctl.h>

#include "thread.h"

typedef struct
{
    char name[THREAD_NAME_MAX_LEN];
    int stack_size;
    int priority;
    int tid;
    int flag;
#if 0
    sigset_t sig;
#endif
    thread_fun_ptr task;
    void **args;
}thread_context_t;

static int get_thread_policy(pthread_attr_t *attr)
{
    int policy;
    int rs = pthread_attr_getschedpolicy( attr, &policy );

    if(rs < 0)
        return -1;
    switch ( policy )
    {
    case SCHED_FIFO:
        break;
    case SCHED_RR:
        break;
    case SCHED_OTHER:
        break;
    default:
        break;
    }

    return policy;
}

static void show_thread_priority( pthread_attr_t *attr, int policy )
{
    int priority = sched_get_priority_max( policy );

    priority = sched_get_priority_min( policy );

}

static int get_thread_priority( pthread_attr_t *attr )
{
    struct sched_param param;

    int rs = pthread_attr_getschedparam( attr, &param );

    if( rs < 0)
        return -1;

    return param.__sched_priority;
}

static void set_thread_priority( pthread_attr_t *attr, int priority )
{

    struct sched_param param;
    int rs = pthread_attr_getschedparam( attr, &param );

    if( rs < 0)
        return;
    param.__sched_priority = priority;
    pthread_attr_setschedparam( attr, &param );

}

static void set_thread_policy( pthread_attr_t *attr,  int policy )
{
    pthread_attr_setschedpolicy( attr, policy );

}

thread_t *ThreadCreate(const char *name, thread_fun_ptr func, void *args)
{
    return ThreadCreate2(name,func,args,0,50);
}

static void *inner_task(void *args)
{
    thread_context_t *th = (thread_context_t *)args;
    if(!th)
    {
        return NULL;
    }

#if 0
    sigemptyset(&(th->sig));
    sigaddset(&(th->sig),SIGUSR1);
    /* /设置该线程的信号屏蔽字为SIGUSR1 */
    pthread_sigmask(SIG_BLOCK,&(th->sig),NULL);
#endif

    th->tid = pthread_self();

    void *ret = th->task(*(th->args));
    return ret;
}


thread_t *ThreadCreate2(const char *name, thread_fun_ptr func, void *args, int stack_size, int priority)
{
#if 0
    pthread_attr_t attr;
    pthread_t tid = 0;
#endif
    thread_context_t *th = (thread_context_t *)calloc(1,sizeof(thread_context_t));
    if(!th)
    {
        return NULL;
    }

    memcpy(th->name,(name ? name : "undef"),sizeof(th->name));
    th->stack_size = stack_size;
    th->priority = priority;
    th->flag = 1;
    th->task = func;
    th->args = &args;
    th->tid = 0;
#if 0
    /* init attribute */
    pthread_attr_init(&attr);
    if(stack_size != 0)
    {
        pthread_attr_setstacksize(&attr,stack_size);
    }

    if(priority > 0)
    {
        set_thread_policy(&attr,SCHED_RR);
        set_thread_priority(&attr,priority);
    }

    int ret = pthread_create(&tid,&attr,inner_task,th);
    if(ret != 0 || tid == 0)
    {
        free(th);
        return NULL;
    }
#endif

#if 0
    /* 注册 */
    struct sigaction act;
    act.sa_handler = SIG_IGN;/*ignore*/
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGUSR1,&act,0);
#endif
    return (thread_t *)th;
}

int ThreadWakeup(thread_t *a_th)
{
    thread_context_t *th = (thread_context_t *)a_th;
    if(th)
        pthread_kill(th->sig,SIGUSR1);
}

int ThreadWait(thread_t *a_th)
{
    int signum;
    thread_context_t *th = (thread_context_t *)a_th;
    sigwait(&(th->sig),&signum);
    return signum;
}

int ThreadStart(thread_t *a_th)
{
    pthread_attr_t attr;
    pthread_t tid = 0;
    thread_context_t *th = (thread_context_t *)a_th;

    if(!th)
        return -1;

    /* init attribute */
    pthread_attr_init(&attr);
    if(th->stack_size != 0)
    {
        pthread_attr_setstacksize(&attr,th->stack_size);
    }

    if(th->priority > 0)
    {
        set_thread_policy(&attr,SCHED_RR);
        set_thread_priority(&attr,th->priority);
    }
#if 1

    int ret = pthread_create(&tid,&attr,inner_task,th);
    if(ret != 0 || tid == 0)
    {
        return -1;
    }
#endif
    return 0;
}

void ThreadStop(thread_t *th)
{
    if(th)
    {
        thread_context_t *ctx = (thread_context_t *)th;
        ctx->flag = 0;
    }
}

static void ignore_signal(int signalValue)
{

    struct sigaction sig;

    sig.sa_handler = SIG_IGN;

    sig.sa_flags = 0;

    sigemptyset(&sig.sa_mask);

    sigaction(signalValue, &sig, NULL);

}

void ThreadDelete(thread_t *th)
{
    if(th)
    {
        thread_context_t *ctx = (thread_context_t *)th;
        ctx->flag = 0;
        ThreadSleep(500);
        ignore_signal(15);//SIGTERM
        kill(ctx->tid,15);
        pthread_join(ctx->tid,NULL);

        free(ctx);
    }
}

void ThreadSleep(int ms)
{
    usleep(ms * 1000);
}

void ThreadSetName(thread_t *th)
{
    if(th)
      {
          thread_context_t *ctx = (thread_context_t *)th;
          prctl(PR_SET_NAME,ctx->name);
      }
}

bool isRunning(thread_t *th)
{
    if(th)
    {
        thread_context_t *ctx = (thread_context_t *)th;
        return (ctx->flag == 1 ? true : false);
    }
    return false;
}
=======
#include <stdio.h>
#include <signal.h>
#include <sched.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include <sys/prctl.h>

#include "thread.h"

typedef struct
{
    char name[THREAD_NAME_MAX_LEN];
    int stack_size;
    int priority;
    int tid;
    int flag;
#if 0
    sigset_t sig;
#endif
    thread_fun_ptr task;
    void **args;
}thread_context_t;

static int get_thread_policy(pthread_attr_t *attr)
{
    int policy;
    int rs = pthread_attr_getschedpolicy( attr, &policy );

    if(rs < 0)
        return -1;
    switch ( policy )
    {
    case SCHED_FIFO:
        break;
    case SCHED_RR:
        break;
    case SCHED_OTHER:
        break;
    default:
        break;
    }

    return policy;
}

static void show_thread_priority( pthread_attr_t *attr, int policy )
{
    int priority = sched_get_priority_max( policy );

    priority = sched_get_priority_min( policy );

}

static int get_thread_priority( pthread_attr_t *attr )
{
    struct sched_param param;

    int rs = pthread_attr_getschedparam( attr, &param );

    if( rs < 0)
        return -1;

    return param.__sched_priority;
}

static void set_thread_priority( pthread_attr_t *attr, int priority )
{

    struct sched_param param;
    int rs = pthread_attr_getschedparam( attr, &param );

    if( rs < 0)
        return;
    param.__sched_priority = priority;
    pthread_attr_setschedparam( attr, &param );

}

static void set_thread_policy( pthread_attr_t *attr,  int policy )
{
    pthread_attr_setschedpolicy( attr, policy );

}

thread_t *ThreadCreate(const char *name, thread_fun_ptr func, void *args)
{
    return ThreadCreate2(name,func,args,0,50);
}

static void *inner_task(void *args)
{
    thread_context_t *th = (thread_context_t *)args;
    if(!th)
    {
        return NULL;
    }

#if 0
    sigemptyset(&(th->sig));
    sigaddset(&(th->sig),SIGUSR1);
    /* /设置该线程的信号屏蔽字为SIGUSR1 */
    pthread_sigmask(SIG_BLOCK,&(th->sig),NULL);
#endif

    th->tid = pthread_self();

    void *ret = th->task(*(th->args));
    return ret;
}


thread_t *ThreadCreate2(const char *name, thread_fun_ptr func, void *args, int stack_size, int priority)
{
#if 0
    pthread_attr_t attr;
    pthread_t tid = 0;
#endif
    thread_context_t *th = (thread_context_t *)calloc(1,sizeof(thread_context_t));
    if(!th)
    {
        return NULL;
    }

    memcpy(th->name,(name ? name : "undef"),sizeof(th->name));
    th->stack_size = stack_size;
    th->priority = priority;
    th->flag = 1;
    th->task = func;
    th->args = &args;
    th->tid = 0;
#if 0
    /* init attribute */
    pthread_attr_init(&attr);
    if(stack_size != 0)
    {
        pthread_attr_setstacksize(&attr,stack_size);
    }

    if(priority > 0)
    {
        set_thread_policy(&attr,SCHED_RR);
        set_thread_priority(&attr,priority);
    }

    int ret = pthread_create(&tid,&attr,inner_task,th);
    if(ret != 0 || tid == 0)
    {
        free(th);
        return NULL;
    }
#endif

#if 0
    /* 注册 */
    struct sigaction act;
    act.sa_handler = SIG_IGN;/*ignore*/
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGUSR1,&act,0);
#endif
    return (thread_t *)th;
}

int ThreadWakeup(thread_t *a_th)
{
    thread_context_t *th = (thread_context_t *)a_th;
    if(th)
        pthread_kill(th->sig,SIGUSR1);
}

int ThreadWait(thread_t *a_th)
{
    int signum;
    thread_context_t *th = (thread_context_t *)a_th;
    sigwait(&(th->sig),&signum);
    return signum;
}

int ThreadStart(thread_t *a_th)
{
    pthread_attr_t attr;
    pthread_t tid = 0;
    thread_context_t *th = (thread_context_t *)a_th;

    if(!th)
        return -1;

    /* init attribute */
    pthread_attr_init(&attr);
    if(th->stack_size != 0)
    {
        pthread_attr_setstacksize(&attr,th->stack_size);
    }

    if(th->priority > 0)
    {
        set_thread_policy(&attr,SCHED_RR);
        set_thread_priority(&attr,th->priority);
    }
#if 1

    int ret = pthread_create(&tid,&attr,inner_task,th);
    if(ret != 0 || tid == 0)
    {
        return -1;
    }
#endif
    return 0;
}

void ThreadStop(thread_t *th)
{
    if(th)
    {
        thread_context_t *ctx = (thread_context_t *)th;
        ctx->flag = 0;
    }
}

static void ignore_signal(int signalValue)
{

    struct sigaction sig;

    sig.sa_handler = SIG_IGN;

    sig.sa_flags = 0;

    sigemptyset(&sig.sa_mask);

    sigaction(signalValue, &sig, NULL);

}

void ThreadDelete(thread_t *th)
{
    if(th)
    {
        thread_context_t *ctx = (thread_context_t *)th;
        ctx->flag = 0;
        ThreadSleep(500);
        ignore_signal(15);//SIGTERM
        kill(ctx->tid,15);
        pthread_join(ctx->tid,NULL);

        free(ctx);
    }
}

void ThreadSleep(int ms)
{
    usleep(ms * 1000);
}

void ThreadSetName(thread_t *th)
{
    if(th)
      {
          thread_context_t *ctx = (thread_context_t *)th;
          prctl(PR_SET_NAME,ctx->name);
      }
}

bool isRunning(thread_t *th)
{
    if(th)
    {
        thread_context_t *ctx = (thread_context_t *)th;
        return (ctx->flag == 1 ? true : false);
    }
    return false;
}
>>>>>>> f6347ac9c15fa717ab49752b17059db469fccf1f
