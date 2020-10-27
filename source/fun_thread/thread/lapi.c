#include <stdio.h>
#include <stdlib.h>

#ifdef __linux__
#include <pthread.h>
#include <sys/prctl.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>
#else

#endif


#include "lapi.h"

struct mutex__
{
    pthread_mutex_t mtx;
    pthread_mutexattr_t mtx_attr;
};

hMutex lapi_mutex_create()
{
    struct mutex__ *mutex = NULL;
    int ret = -1;

    mutex = (struct mutex__ *)calloc(1,sizeof(struct mutex__));
    if(!mutex)
    {
        printf("[mutex] mutex no memory\n");
        return NULL;
    }

    ret = pthread_mutexattr_init(&(mutex->mtx_attr));
    if(ret != 0)
    {
        printf("[mutex] mutex attr init failed\n");
        return NULL;
    }
    /* 线程用*/
    pthread_mutexattr_setpshared(&(mutex->mtx_attr),PTHREAD_PROCESS_PRIVATE);
    /*设置健壮性*/
    pthread_mutexattr_setrobust(&(mutex->mtx_attr),PTHREAD_MUTEX_ROBUST);

    ret = pthread_mutex_init(&(mutex->mtx),&(mutex->mtx_attr));
    if(ret != 0)
    {
        printf("[mutex] mutex init failed\n");
        pthread_mutexattr_destroy(&(mutex->mtx_attr));
        return NULL;
    }

    return (hMutex)mutex;
}

int lapi_mutex_destroy(hMutex mtx)
{
    int ret = -1;
    struct mutex__ *mutex = (struct mutex__ *)mtx;
    if(!mutex)
        return -1;
    ret = pthread_mutexattr_destroy(&mutex->mtx_attr);
    if(ret != 0)
    {
        return -1;
    }
    ret = pthread_mutex_destroy(&mutex->mtx);
    if(ret != 0)
        return -1;

    free(mtx);
    return 0;
}

int lapi_mutex_lock(hMutex mtx)
{
    struct mutex__ *mutex = (struct mutex__ *)mtx;
    if(!mutex)
        return -1;
    int ret = pthread_mutex_lock(&mutex->mtx);
    printf("ret = %d\n",ret);
    if(ret == EOWNERDEAD)
    {
        /* 恢复该锁的一致性*/
        pthread_mutex_consistent(&mutex->mtx);
        return 0;
    }
    return ret;
}

int lapi_mutex_trylock(hMutex mtx)
{
    struct mutex__ *mutex = (struct mutex__ *)mtx;
    if(!mutex)
        return -1;
    /* 如果锁被占用 返回EBUSY 而不是阻塞*/
    int ret = pthread_mutex_trylock(&mutex->mtx);
    if(ret == EBUSY)
        return -1;
    return ret;
}

int lapi_mutex_unlock(hMutex mtx)
{
    struct mutex__ *mutex = (struct mutex__ *)mtx;
    if(!mutex)
        return -1;
    return pthread_mutex_unlock(&mutex->mtx);
}

struct cond__
{
    pthread_cond_t cnd;
    pthread_condattr_t cnd_attr;
    pthread_mutex_t mtx;
    pthread_mutexattr_t mtx_attr; /*这里不用struct mutex__ 内部解耦*/
};

hCond lapi_cond_create()
{
    struct cond__ *cond = (struct cond__ *)calloc(1,sizeof(struct cond__));
    if(!cond)
        return NULL;
    pthread_mutexattr_init(&cond->mtx_attr);
    /* 线程用*/
    pthread_mutexattr_setpshared(&cond->mtx_attr,PTHREAD_PROCESS_PRIVATE);
    /*设置健壮性*/
    pthread_mutexattr_setrobust(&(cond->mtx_attr),PTHREAD_MUTEX_ROBUST);

    pthread_mutex_init(&cond->mtx,&cond->mtx_attr);

    pthread_condattr_init(&cond->cnd_attr);
    pthread_condattr_setpshared(&cond->cnd_attr,PTHREAD_PROCESS_PRIVATE);

    pthread_cond_init(&cond->cnd,&cond->cnd_attr);

    return (hCond)cond;
}
int lapi_cond_destroy(hCond cnd)
{
    struct cond__ *cond = (struct cond__ *)cnd;
    if(!cond)
        return -1;
    pthread_mutexattr_destroy(&cond->mtx_attr);
    pthread_mutex_destroy(&cond->mtx);
    pthread_condattr_destroy(&cond->cnd_attr);
    pthread_cond_destroy(&cond->cnd);

    free(cond);
    return 0;
}
int lapi_cond_signal(hCond cnd)
{
    struct cond__ *cond = (struct cond__ *)cnd;
    if(!cond)
        return -1;
    return pthread_cond_signal(&cond->cnd);
}
int lapi_cond_broadcast(hCond cnd)
{
    struct cond__ *cond = (struct cond__ *)cnd;
    if(!cond)
        return -1;
    return pthread_cond_broadcast(&cond->cnd);
}
int lapi_cond_wait(hCond cnd)
{
    struct cond__ *cond = (struct cond__ *)cnd;
    if(!cond)
        return -1;
    return pthread_cond_wait(&cond->cnd,&cond->mtx);
}
int lapi_cond_waittimeout(hCond cnd,int ms)
{
    struct cond__ *cond = (struct cond__ *)cnd;
    if(!cond)
        return -1;
    struct timespec tv;
    tv.tv_sec = (ms / 1000);
    tv.tv_nsec = (ms % 1000) * 1000;

    return pthread_cond_timedwait(&cond->cnd,&cond->mtx,&tv);
}
int lapi_cond_lock(hCond cnd)
{
    struct cond__ *cond = (struct cond__ *)cnd;
    if(!cond)
        return -1;
    int ret = pthread_mutex_lock(&cond->mtx);
    #if 0
    printf("ret = %d\n",ret);
    if(ret == EOWNERDEAD)
    {
        pthread_mutex_consistent(&cond->mtx);
        return 0;
    }
    #endif // 0
    return ret;
}
int lapi_cond_unlock(hCond cnd)
{
    struct cond__ *cond = (struct cond__ *)cnd;
    if(!cond)
        return -1;
    return pthread_mutex_unlock(&cond->mtx);
}


struct rw_lock__
{
    pthread_rwlock_t lck;
    pthread_rwlockattr_t lck_attr;
};

hRwlock lapi_rwlock_create()
{
    struct rw_lock__ *rwlock = (struct rw_lock__ *)calloc(1,sizeof(struct rw_lock__));
    if(!rwlock)
        return NULL;

    pthread_rwlockattr_init(&rwlock->lck_attr);
    pthread_rwlockattr_setpshared(&rwlock->lck_attr,PTHREAD_PROCESS_PRIVATE);

    pthread_rwlock_init(&rwlock->lck,&rwlock->lck_attr);

    return (hRwlock)rwlock;
}

int lapi_rwlock_rdlock(hRwlock lock)
{
    struct rw_lock__ *rwlock = (struct rw_lock__ *)lock;
    if(!rwlock)
        return -1;
    return pthread_rwlock_rdlock(&rwlock->lck);
}

int lapi_rwlock_wrlock(hRwlock lock)
{
    struct rw_lock__ *rwlock = (struct rw_lock__ *)lock;
    if(!rwlock)
        return -1;
    return pthread_rwlock_wrlock(&rwlock->lck);
}

int lapi_rwlock_unlock(hRwlock lock)
{
    struct rw_lock__ *rwlock = (struct rw_lock__ *)lock;
    if(!rwlock)
        return -1;
    return pthread_rwlock_unlock(&rwlock->lck);
}

int lapi_rwlock_destroy(hRwlock lock)
{
    struct rw_lock__ *rwlock = (struct rw_lock__ *)lock;
    if(!rwlock)
        return -1;

    pthread_rwlockattr_destroy(&rwlock->lck_attr);
    pthread_rwlock_destroy(&rwlock->lck);
    return 0;
}

int lapi_rwlock_tryrdlock(hRwlock lock)
{
    struct rw_lock__ *rwlock = (struct rw_lock__ *)lock;
    if(!rwlock)
        return -1;
    return pthread_rwlock_tryrdlock(&rwlock->lck);
}

int lapi_rwlock_trywrlock(hRwlock lock)
{
    struct rw_lock__ *rwlock = (struct rw_lock__ *)lock;
    if(!rwlock)
        return -1;
    return pthread_rwlock_trywrlock(&rwlock->lck);
}

struct sem__
{
	sem_t sm;
};

hSem lapi_sem_create()
{
	struct sem__ *sem = (struct sem__ *)calloc(1,sizeof(struct sem__));
	if(!sem)
		return NULL;
	int ret = sem_init(&sem->sm,0,1);
	if(ret < 0)
	{
		free(sem);
		return NULL;
	}
	
	return (hSem)sem;
	
}
int lapi_sem_destroy(hSem sem)
{
	struct sem__ *s_sem = (struct sem__ *)sem;
	if(!s_sem)
		return -1;
	int ret = sem_destroy(&s_sem->sm);
	if(ret < 0)
		return -1;
	free(s_sem);
	return 0;
}
int lapi_sem_put(hSem sem)
{
	struct sem__ *s_sem = (struct sem__ *)sem;
	if(!s_sem)
		return -1;
	
	return sem_wait(&s_sem->sm);
}
int lapi_sem_put_timeout(hSem sem,int ms)
{
	struct sem__ *s_sem = (struct sem__ *)sem;
	if(!s_sem)
		return -1;
	struct timespec tv;
	tv.tv_sec = ms / 1000;
	tv.tv_nsec = (ms % 1000) * 1000;
	
	return sem_timedwait(&s_sem->sm,&tv);
}
int lapi_sem_get(hSem sem)
{
	struct sem__ *s_sem = (struct sem__ *)sem;
	if(!s_sem)
		return -1;
	
	return sem_post(&s_sem->sm);
}

#define THREAD_FLAGS_ISRUNNING  (1 << 0)
#define THREAD_FLAGS_ISDETACH   (1 << 1)
struct thread__
{
    pthread_t tid;
    pid_t pid;
    pthread_attr_t attr;
    int flags;
    lapi_thread_func task;
    void *args;
};

hThread lapi_thread_create(lapi_thread_func func,void *args,int stack_size)
{
    return lapi_thread_create_with_priority(func,args,stack_size,50);
}

void *inner_task(void *args)
{
    struct thread__ *th = (struct thread__ *)args;
    if(!th)
        return NULL;

    th->tid = pthread_self();
    th->pid = getpid();

    void *ret = th->task((hThread)th,th->args);

    th->flags &= ~THREAD_FLAGS_ISRUNNING;
    return ret;
}


hThread lapi_thread_create_with_priority(lapi_thread_func func,void *args,int stack_size,int priority)
{
    struct thread__ *thread = (struct thread__ *)calloc(1,sizeof(struct thread__));
    if(!thread)
        return NULL;

    thread->flags |= THREAD_FLAGS_ISRUNNING;
    thread->pid = 0;
    thread->tid = 0;
    thread->task = func;
    thread->args = args;
    pthread_attr_init(&thread->attr);
    if(stack_size > 0)
    {
        pthread_attr_setstacksize(&thread->attr,stack_size);
    }

    if(priority > 0)
	{
		pthread_attr_setschedpolicy(&thread->attr,SCHED_RR); /*调度策略*/
		struct sched_param param;
		pthread_attr_getschedparam(&thread->attr,&param);
		param.__sched_priority = priority;
		pthread_attr_setschedparam(&thread->attr,&param);
	}

    pthread_t tid;
    int ret = pthread_create(&tid,&thread->attr,inner_task,thread);
    if(ret != 0)
    {
        pthread_attr_destroy(&thread->attr);
        free(thread);
        return NULL;
    }
    return (hThread)thread;
}

int lapi_thread_destroy(hThread th)
{
    struct thread__ *thread = (struct thread__ *)th;
    if(!thread)
        return -1;

    if(thread->flags & THREAD_FLAGS_ISDETACH)
    {
        pthread_attr_destroy(&thread->attr);
        thread->flags &= ~THREAD_FLAGS_ISRUNNING;
        free(thread);
        return 0;
    }

    lapi_thread_kill(th);
    pthread_join(thread->tid,NULL);
    return 0;
}

int lapi_thread_isrunning(hThread th)
{
    struct thread__ *thread = (struct thread__ *)th;
    if(thread)
    {
        return (thread->flags & THREAD_FLAGS_ISRUNNING) ? 1 : 0;
    }
    return 0;
}
void lapi_thread_setname(const char *name)
{
    prctl(PR_SET_NAME, name, 0, 0, 0);
}

int lapi_thread_kill(hThread th)
{
    struct thread__ *thread = (struct thread__ *)th;
    if(!thread)
        return -1;
    return pthread_kill(thread->tid,SIGKILL);
}
int lapi_thread_cancel(hThread th)
{
    struct thread__ *thread = (struct thread__ *)th;
    if(!thread)
        return -1;
    return pthread_cancel(thread->tid);
}
int lapi_thread_detach(hThread th)
{
    struct thread__ *thread = (struct thread__ *)th;
    if(!thread)
        return -1;

    pthread_detach(thread->tid);
    thread->flags |= THREAD_FLAGS_ISDETACH;
    return 0;
}

void lapi_sleep(int ms)
{
    usleep(ms * 1000);
}
