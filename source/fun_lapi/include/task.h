#ifndef __LAPI_H
#define __LAPI_H

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


/**
mutex
*/
declear_handler(hMutex);

hMutex lapi_mutex_create();
int lapi_mutex_lock(hMutex mtx);
int lapi_mutex_trylock(hMutex mtx);
int lapi_mutex_unlock(hMutex mtx);
int lapi_mutex_destroy(hMutex mtx);

/**
cond
*/
declear_handler(hCond);

hCond lapi_cond_create();
int lapi_cond_destroy(hCond cnd);
int lapi_cond_signal(hCond cnd);
int lapi_cond_broadcast(hCond cnd);
int lapi_cond_wait(hCond cnd,hMutex mtx);
int lapi_cond_waittimeout(hCond cnd, hMutex mtx, int ms);


/**
spinlock
*/
declear_handler(hSpin);

hSpin lapi_spin_create();
int lapi_spin_destroy(hSpin spin);
int lapi_spin_lock(hSpin spin);
int lapi_spin_unlock(hSpin spin);
int lapi_spin_trylock(hSpin spin);

/**
rwlock
*/
declear_handler(hRwlock);

hRwlock lapi_rwlock_create();
int lapi_rwlock_rdlock(hRwlock lock);
int lapi_rwlock_wrlock(hRwlock lock);
int lapi_rwlock_unlock(hRwlock lock);
int lapi_rwlock_destroy(hRwlock lock);
int lapi_rwlock_tryrdlock(hRwlock lock);
int lapi_rwlock_trywrlock(hRwlock lock);

/**
semaphore
*/
declear_handler(hSem);

hSem lapi_sem_create();
int lapi_sem_destroy(hSem sem);
int lapi_sem_put(hSem sem);
int lapi_sem_put_timeout(hSem sem,int ms);
int lapi_sem_get(hSem sem);


/**
pthread
*/
declear_handler(hThread);

typedef void *(*lapi_thread_func)(hThread th,void *args);
hThread lapi_thread_create(lapi_thread_func func,void *args,int stack_size);
hThread lapi_thread_create_with_priority(lapi_thread_func func,void *args,int stack_size,int priority);

int lapi_thread_destroy(hThread th);

int lapi_thread_isrunning(hThread th);
void lapi_thread_setname(const char *name);

int lapi_thread_kill(hThread th);
int lapi_thread_cancel(hThread th);
int lapi_thread_detach(hThread th);

void lapi_sleep(int ms);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __LAPI_PTHREAD_H
