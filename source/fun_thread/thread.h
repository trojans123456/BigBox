#ifndef __THREAD_H
#define __THREAD_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define THREAD_NAME_MAX_LEN     32

typedef struct {int unused;} thread_t;

typedef void *(*thread_fun_ptr)(void *);

thread_t *ThreadCreate(const char *name, thread_fun_ptr func, void *args);

thread_t *ThreadCreate2(const char *name,thread_fun_ptr func,void *args,int stack_size,int priority);

void ThreadDelete(thread_t *th);

int ThreadStart(thread_t *th);
void ThreadStop(thread_t *th);

void ThreadSleep(int ms);
void ThreadSetName(thread_t *th);

bool isRunning(thread_t *th);

#ifdef __cplusplus
}
#endif

#endif
