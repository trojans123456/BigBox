#ifndef __RESALLOC_H
#define __RESALLOC_H

#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/************ rwlock **************/
typedef struct
{
    pthread_rwlock_t rwc;
    pthread_t mtid;
}res_rw_t;//读写锁

/* 如果不创建。mtid = pthread_self()*/
res_rw_t *create_rw();
void delete_rw(res_rw_t *rw);

//towr 1读 0 写
#define RWLOCK_WRITE    1
#define RWLOCK_READ     0
int rw_lock(res_rw_t *g_this,int towr);
int rw_tryLock(res_rw_t *g_this,int towr);
void rw_unlock(res_rw_t *g_this);

int rw_requestW(res_rw_t *g_this);
int rw_tryW(res_rw_t *g_this);
int rw_requestR(res_rw_t *g_this);
int rw_tryR(res_rw_t *g_this);
void rw_release(res_rw_t *g_this);


/*********** mutex **************/
typedef struct
{
    pthread_mutex_t m;
}res_mtx_t;

res_mtx_t *create_mtx();
void delete_mtx(res_mtx_t *mtx);

int mtx_lock(res_mtx_t *mtx);
int mtx_tryLock(res_mtx_t *mtx);
int mtx_unlock(res_mtx_t *mtx);

/********** cond *************/
typedef struct
{
    pthread_mutex_t mtx;
    pthread_cond_t cond;
}res_cond_t;

res_cond_t *create_cond();
void delete_cond(res_cond_t *cond);

int res_condLock(res_cond_t *g_this);
int res_condUnlock(res_cond_t *g_this);

int res_condSignal(res_cond_t *g_this);
int res_condBroadcast(res_cond_t *g_this);
int res_condWait(res_cond_t *g_this);
int res_condTimeWait(res_cond_t *g_this,unsigned s,unsigned ms);

/**************** semaphore ***************/
typedef struct
{
#define SEM_NAME_MAX_LEN    16
    char name[SEM_NAME_MAX_LEN];
    sem_t *sem;
}res_sem_t;

res_sem_t *create_sem(const char *name);
void delete_sem(res_sem_t *g_this);

int res_semWait(res_sem_t *g_this,int ms);
int res_semRelease(res_sem_t *g_this);
int res_semTrywait(res_sem_t *g_this);

/**************** spin lock **************/
typedef struct
{

}res_spin_t;

res_spin_t *create_spin();
int res_SpinLock(res_spin_t *spin);
int res_SpinUnlock(res_spin_t *spin);
int res_SpinTryLock(res_spin_t *spin);

#ifdef __cplusplus
}
#endif

#endif
