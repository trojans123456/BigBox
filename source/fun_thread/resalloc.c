#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#include "resalloc.h"

/* Èç¹û²»´´½¨¡£mtid = pthread_self()*/
res_rw_t *create_rw()
{
    res_rw_t *rw_ptr = (res_rw_t *)calloc(1,sizeof(res_rw_t));
    if(!rw_ptr)
    {
        printf("create res rw failed\n");
        return NULL;
    }
    if(pthread_rwlock_init(&rw_ptr->rwc,NULL)) //sucess return 0
    {
        free(rw_ptr);
        return NULL;
    }
    rw_ptr->mtid = 0;
    return rw_ptr;
}
void delete_rw(res_rw_t *rw)
{
    if(rw)
    {
        pthread_rwlock_wrlock(&rw->rwc);
        pthread_rwlock_destroy(&rw->rwc);

        free(rw);
        rw = NULL;
    }
}

int rw_lock(res_rw_t *g_this,int towr)
{

    if(towr == RWLOCK_WRITE)
    {
        return rw_requestW(g_this);
    }
    else if(towr == RWLOCK_READ)
    {
        return rw_requestR(g_this);
    }


    return -1;
}
int rw_tryLock(res_rw_t *g_this,int towr)
{
    if(towr == RWLOCK_WRITE)
        return rw_tryW(g_this);
    return rw_tryR(g_this);
}
void rw_unlock(res_rw_t *g_this)
{
    rw_release(g_this);
}

int rw_requestW(res_rw_t *g_this)
{
    if(!g_this)
        return -1;


    int rez = 0;
    unsigned short tm = 0;

    if(g_this->mtid && (g_this->mtid == pthread_self()))
        rez = EDEADLK; //ËÀËø

    if(!tm)
        rez = pthread_rwlock_wrlock(&g_this->rwc);
    else
    {
        struct timespec wtm;
        clock_gettime(CLOCK_REALTIME,&wtm);
        wtm.tv_nsec += 1000000 * (tm % 1000);
        wtm.tv_sec += tm/1000 + wtm.tv_nsec/1000000000;
        wtm.tv_nsec = wtm.tv_nsec%1000000000;
        rez = pthread_rwlock_timedwrlock(&g_this->rwc, &wtm);
    }

    if(rez == EDEADLK)
        return -1;
    else if(tm && (rez == ETIMEDOUT))
    {
        return -1;
    }
    g_this->mtid = pthread_self();
    return 1;
}
int rw_tryW(res_rw_t *g_this)
{
    if(!g_this)
        return -1;

    int rez = pthread_rwlock_trywrlock(&g_this->rwc);
    if(rez == EBUSY || rez == EDEADLK)
        return -1;
    return 1;
}
int rw_requestR(res_rw_t *g_this)
{
    if(!g_this)
        return -1;
    int rez = 0;
    unsigned short tm = 0;

    if(g_this->mtid && g_this->mtid == pthread_self())
        rez = EDEADLK;
    else

        if(!tm)
            rez = pthread_rwlock_rdlock(&g_this->rwc);
        else
        {
            struct timespec wtm;

            clock_gettime(CLOCK_REALTIME, &wtm);
            wtm.tv_nsec += 1000000*(tm%1000);
            wtm.tv_sec += tm/1000 + wtm.tv_nsec/1000000000;
            wtm.tv_nsec = wtm.tv_nsec%1000000000;
            rez = pthread_rwlock_timedrdlock(&g_this->rwc,&wtm);
        }
    if(rez == EDEADLK)
        return -1;
    else if(tm && rez == ETIMEDOUT)
        return -1;
    return 1;
}
int rw_tryR(res_rw_t *g_this)
{
    if(!g_this)
        return -1;
    int rez = pthread_rwlock_tryrdlock(&g_this->rwc);
    if(rez == EBUSY || rez == EDEADLK)
        return -1;
    return 1;
}
void rw_release(res_rw_t *g_this)
{
    if(g_this)
    {
        pthread_rwlock_unlock(&g_this->rwc);
        if(g_this->mtid == pthread_self())
            g_this->mtid = 0;
    }
}

/********************* mutex *******************/
/** 服务于共享资源的  */
res_mtx_t *create_mtx()
{
    res_mtx_t *mtx = (res_mtx_t *)calloc(1,sizeof(res_mtx_t));
    if(!mtx)
    {
        return NULL;
    }

    pthread_mutexattr_t attrM;
    pthread_mutexattr_init(&attrM);
    pthread_mutex_init(&mtx->m,&attrM);
    pthread_mutexattr_destroy(&attrM);

    return mtx;
}
void delete_mtx(res_mtx_t *mtx)
{
    if(mtx)
    {
        pthread_mutex_destroy(&mtx->m);
        free(mtx);
    }
}

int mtx_lock(res_mtx_t *mtx)
{
    if(mtx)
    {
        return pthread_mutex_lock(&mtx->m);
    }
    return -1;
}
int mtx_tryLock(res_mtx_t *mtx)
{
    if(mtx)
    {
        return pthread_mutex_trylock(&mtx->m);
    }
    return -1;
}
int mtx_unlock(res_mtx_t *mtx)
{
    if(mtx)
    {
        return pthread_mutex_unlock(&mtx->m);
    }
    return -1;
}

/*************** cond *****************/
res_cond_t *create_cond()
{
    res_cond_t *cond = (res_cond_t *)calloc(1,sizeof(res_cond_t));
    if(!cond)
    {
        return NULL;
    }
    pthread_mutex_init(&cond->mtx,NULL);
    pthread_cond_init(&cond->cond,NULL);
    return cond;
}
void delete_cond(res_cond_t *cond)
{
    if(cond)
    {
        pthread_mutex_destroy(&cond->mtx);
        pthread_cond_destroy(&cond->cond);

        free(cond);
    }
}

int res_condLock(res_cond_t *g_this)
{
    if(g_this)
    {
        return pthread_mutex_lock(&g_this->mtx);
    }
    return -1;
}
int res_condUnlock(res_cond_t *g_this)
{
    if(g_this)
    {
        return pthread_mutex_unlock(&g_this->mtx);
    }
    return -1;
}

int res_condSignal(res_cond_t *g_this)
{
    if(g_this)
    {
        return pthread_cond_signal(&g_this->cond);
    }
    return -1;
}
int res_condBroadcast(res_cond_t *g_this)
{
    if(g_this)
    {
        return pthread_cond_broadcast(&g_this->cond);
    }
    return -1;
}
int res_condWait(res_cond_t *g_this)
{
    if(g_this)
    {
        return pthread_cond_wait(&g_this->cond,&g_this->mtx);
    }
    return -1;
}
int res_condTimeWait(res_cond_t *g_this,unsigned s,unsigned ms)
{
    if(g_this)
    {
        struct timespec abstime;
        clock_gettime(CLOCK_REALTIME,&abstime);
        abstime.tv_sec += s;
        abstime.tv_nsec += ((long)ms) * 1000 * 1000;

        return pthread_cond_timedwait(&g_this->cond,&g_this->mtx,&abstime);
    }
    return -1;
}

/************ sem ***************/
/* 服务于多线程的执行顺序的 */
res_sem_t *create_sem(const char *name)
{
    if(!name)
        return NULL;

    res_sem_t *sem = (res_sem_t *)calloc(1,sizeof(res_sem_t));
    if(!sem)
    {
        return NULL;
    }
    memcpy(sem->name,name,sizeof(sem->name));
    sem->sem = sem_open(name,O_CREAT,0644,1);
    if(!sem->sem)
    {
        free(sem);
        return NULL;
    }
    return sem;
}
void delete_sem(res_sem_t *g_this)
{
    if(g_this)
    {
        sem_close(g_this->sem);
        sem_unlink(g_this->name);
        free(g_this);
    }
}

int res_semWait(res_sem_t *g_this,int ms)
{
    if(g_this)
    {
        if(-1 == ms)
        {
            return sem_wait(g_this->sem);
        }
        else
        {
            struct timespec ts;
            ts.tv_sec = ms / 1000;
            ts.tv_nsec = (ms % 1000) * 1000000L;

            return sem_timedwait(g_this->sem,&ts);
        }
    }
    return 0;
}
int res_semRelease(res_sem_t *g_this)
{
    if(g_this)
    {
        return sem_post(g_this->sem);
    }
    return 0;
}
int res_semTrywait(res_sem_t *g_this)
{
    if(g_this)
    {
        if(g_this->sem)
        {
            return sem_trywait(g_this->sem);
        }
    }
    return 0;
}
