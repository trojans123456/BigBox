#include <fcntl.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "timerfd.h"

int set_non_block(int fd)
{
    int flags = fcntl(fd,F_GETFL,0);
    flags |= O_NONBLOCK;

    if(fcntl(fd,F_SETFL,flags) == -1)
        return -1;

    return 1;
}

timerfd_t *create_timer(char *name,timer_fun_t fun,void *arg,double interval,int repeat)
{
    if(!fun)
        return NULL;

    timerfd_t *timer = (timerfd_t *)calloc(1,sizeof(timerfd_t));
    if(!timer)
    {
        printf("create timer faild\n");
        return NULL;
    }
    timer->mTimerid = timerfd_create(CLOCK_MONOTONIC,0);//CLOCK_REALTIME
    if(timer->mTimerid < 0)
    {
        free(timer);
        return NULL;
    }

    memcpy(timer->mName,name,sizeof(timer->mName));
    timer->mTimerInterval = interval;
    timer->mRepeat = repeat;
    timer->mTimerFun = fun;
    timer->mArg = arg;

    set_non_block(timer->mTimerid);

    return timer;
}

void delete_timer(timerfd_t *g_this)
{
    if(g_this)
    {
        if(g_this->mTimerid > 0)
            close(g_this->mTimerid);

        free(g_this);
        g_this = NULL;
    }
}

int set_timer_interval(timerfd_t *g_this,double interval)
{
    if(g_this)
    {
        g_this->mTimerInterval = interval;
        return timer_start(g_this);
    }
    return -1;
}

int timer_start(timerfd_t *tm)
{
    if(tm)
    {
        struct itimerspec l_time = {0};
        //第一次超时时间
        l_time.it_value.tv_sec = (int)tm->mTimerInterval;
        l_time.it_value.tv_nsec = (tm->mTimerInterval - (int)(tm->mTimerInterval)) * 1000000;

        if(tm->mRepeat)
        {
            //如果为0，表示不是周期性 (定时间隔)
            //printf("repeat\n");
            l_time.it_interval.tv_sec = l_time.it_value.tv_sec;
            l_time.it_interval.tv_nsec = l_time.it_value.tv_nsec;
        }
        return timerfd_settime(tm->mTimerid,0,&l_time,NULL);
    }
    return -1;
}
void timer_stop(timerfd_t *tm)
{
    if(tm)
    {
        close(tm->mTimerid);
        tm->mTimerid = -1;
    }
}

int timer_exec(timerfd_t *tm)
{
    if(tm)
    {
        char buf[32] = "";
        //read到的是计时器的超时超时次数
        int ret = read(tm->mTimerid,buf,sizeof(buf));
        //printf("buf = %d \n",atoi(buf));
        if((ret > 0) && tm->mTimerFun)
        {
            return tm->mTimerFun(tm->mArg);
        }
    }
    return -1;
}


/**************** timer 管理 ****************/
#if 1
static unsigned int hash_ops_string(const char *a_string)
{
    unsigned int h;
    for(h = 0; *a_string != '\0'; a_string++)
    {
        h = *a_string + 31 * h;
    }
    return h;
}

static void* hash_key_dup(const void *key)
{
    if(key)
    {
        #if 0
        int key_len = sizeof(int);
        char *ptr = (char *)malloc(key_len);
        if(ptr)
        {
            *ptr = *((int *)key);
            return (void *)ptr;
        }
        #endif
        int key_len = strlen(key);
        char *ptr = (char *)malloc(key_len);
        if(ptr)
        {
            memcpy(ptr,key,key_len);
            return (void *)ptr;
        }
    }
    return NULL;
}
static void hash_key_free(void *key)   //key
{
    if(key)
    {
        free(key);
    }
}

static void* hash_data_dup(const void *value) //value
{
    if(value)
    {
        timerfd_t *ptr = (timerfd_t*)malloc(sizeof(timerfd_t));
        if(ptr)
        {
            ptr->mArg = ((timerfd_t *)value)->mArg;
            strcpy(ptr->mName,(const char *)((timerfd_t *)value)->mName);
            ptr->mRepeat = ((timerfd_t *)value)->mRepeat;
            ptr->mTimerFun = ((timerfd_t *)value)->mTimerFun;
            ptr->mTimerid = ((timerfd_t *)value)->mTimerid;
            ptr->mTimerInterval = ((timerfd_t *)value)->mTimerInterval;

            return ptr;
        }
    }
    return NULL;
}
static void hash_data_free(void *value)
{
    if(value)
    {
        free(value);
    }
}

static hash_ops_t tmap_hash_ops =
{
     (void *)&hash_ops_string,
     (void *)&strcmp,
     (void *)&hash_key_dup,
     (void *)&hash_key_free,
     0,0
     //(void *)&hash_data_dup,
     //(void *)&hash_data_free
};
#endif
timermap_t *create_timermap(int timer_max)
{
    timermap_t *tmap = (timermap_t *)calloc(1,sizeof(timermap_t));
    if(!tmap)
    {
        printf("create timer map failed\n");
        return NULL;
    }
    tmap->mEpollfd = epoll_create(timer_max);
    //printf("epollfd = %d \n",tmap->mEpollfd);
    if(tmap->mEpollfd < 0)
    {
        free(tmap);
        return NULL;
    }
    //tmap->mHash = hash_new(0,hash_ops);
    tmap->mHash = hash_new(0,&tmap_hash_ops);
    if(!tmap->mHash)
    {
        goto err;
    }
    tmap->mActive = 1;
    tmap->mMaxCnt = timer_max;

    return tmap;

err:
    if(tmap->mEpollfd > 0)
        close(tmap->mEpollfd);
    if(tmap)
        free(tmap);
    return NULL;
}
void delete_timermap(timermap_t *g_this)
{
    if(g_this)
    {
        if(g_this->mEpollfd > 0)
            close(g_this->mEpollfd);
        if(g_this->mHash)
        {
            hash_delete(g_this->mHash);
        }
    }
}

int addTimer(timermap_t *g_this,timerfd_t *tm)
{
    if(g_this && tm)
    {
        struct epoll_event event;
        event.data.fd = tm->mTimerid;
        event.events = EPOLLIN | EPOLLET;

        char num[32] = "";
        sprintf(num,"%d",tm->mTimerid);

        hash_add(g_this->mHash,num,tm);
        printf("timerid = %d timername = %s\n",tm->mTimerid,tm->mName);
        return epoll_ctl(g_this->mEpollfd,EPOLL_CTL_ADD,tm->mTimerid,&event);
    }
    return -1;
}
int delTimer(timermap_t *g_this,timerfd_t *tm)
{
    if(g_this && tm)
    {
        struct epoll_event event;
        event.data.fd = tm->mTimerid;
        event.events = EPOLLIN | EPOLLET;

        epoll_ctl(g_this->mEpollfd,EPOLL_CTL_DEL,tm->mTimerid,&event);

        char num[32] = "";
        sprintf(num,"%d",tm->mTimerid);

        hash_del(g_this->mHash,num);
    }
    return -1;
}
void timermap_listen(timermap_t *g_this)
{
    if(!g_this)
    {
        return ;
    }

    struct epoll_event events[g_this->mMaxCnt];
    int fd_num = -1;
    int pos = 0;
    hash_iter_t *iter = NULL;
    char num[32] = "";

    for(;;)
    {
        if(g_this->mActive == 0)
            break;

        fd_num = epoll_wait(g_this->mEpollfd,events,g_this->mMaxCnt,-1);
        //printf("fd_num = %d \n",fd_num);
        if(fd_num <= 0)
        {
            continue;
        }

        for(pos = 0;pos < fd_num;++pos)
        {
            if(events[pos].events & EPOLLIN)
            {
                printf("fd = %d fd_num = %d\n",events[pos].data.fd,fd_num);
                sprintf(num,"%d",events[pos].data.fd);
                iter = hash_lookup(g_this->mHash,num);
                if(iter && iter->data)
                {
                   // printf("exec fun\n");
                    //((timerfd_t *)(iter->data))->mTimerFun(((timerfd_t *)(iter->data))->mArg);
                    timer_exec(((timerfd_t *)(iter->data)));
                }
            }
        }
    }
}
