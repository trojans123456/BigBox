#ifndef E_TIMER_H_
#define E_TIMER_H_

#include "timerfd_hash.h"

#ifdef __cplusplus
extern "C" {
#endif



#define TIMER_NAME_MAX_LEN  32
typedef int (*timer_fun_t)(void *);
typedef struct
{
    int mTimerid;
    double mTimerInterval;//
    int mRepeat;//once or period
    timer_fun_t mTimerFun;
    void *mArg;
    char mName[TIMER_NAME_MAX_LEN];
}timerfd_t;

timerfd_t *create_timer(char *name,timer_fun_t fun,void *arg,double interval,int repeat);
void delete_timer(timerfd_t *g_this);

int set_timer_interval(timerfd_t *g_this,double interval);

int timer_start(timerfd_t *tm);
void timer_stop(timerfd_t *tm);
int timer_exec(timerfd_t *tm);

int set_non_block(int fd);


typedef struct
{
    int mEpollfd;
    int mActive;
    int mMaxCnt;
    hash_t *mHash;
}timermap_t;


timermap_t *create_timermap(int timer_max);
void delete_timermap(timermap_t *g_this);

int addTimer(timermap_t *g_this,timerfd_t *tm);
int delTimer(timermap_t *g_this,timerfd_t *tm);
void timermap_listen(timermap_t *g_this);

#ifdef __cplusplus
}
#endif

#endif // E_TIMER_H_
