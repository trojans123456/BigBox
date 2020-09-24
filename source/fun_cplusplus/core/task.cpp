
#include <pthread.h>
#include <signal.h>
#ifndef __WIN32__
#include <sys/prctl.h>
#include <sys/resource.h>
#endif
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>


#include <map>
#include "resalloc.h"
#include "task.h"

using std::map;

using namespace ns_core;

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))



class STask
{
public:
    /*分离*/          /*结束*/
    enum Flgs {Detached = 0x01,FinishTask = 0x02};

    STask():thr(0),policy(0),prior(0),tid(0),flgs(0){}
    STask(pthread_t ithr,char ipolicy,char iprior):thr(ithr),policy(ipolicy),prior(iprior),tid(0),flgs(0){}

    /*属性*/
    string          path;/*线程路径*/
    pthread_t       thr;/*pid*/
    unsigned char   policy,prior;
    pid_t           tid;
    void *(*task)(void *);
    void            *taskArg;
    unsigned int    flgs;

};


static void *taskWrap(void *args);

static map<string,STask> mTasks;
static ResRW	taskRes;


void Task::setThreadName(string name)
{
    if(name.empty())
        return ;

    prctl(PR_SET_NAME,name.c_str());
}

void Task::taskCreate(const string &path, int priority, void *(*start_routine)(void *), void *arg, int wtm,pthread_attr_t *pAttr, bool *startSt)
{
    int detachStat = 0;
    pthread_t procPthr;
    pthread_attr_t locPAttr, *pthr_attr;

    map<string,STask>::iterator ti;

    ResAlloc res(taskRes, true); /*加锁 执行完自动析构*/

    for(time_t c_tm = time(NULL); (ti=mTasks.find(path)) != mTasks.end(); )
    {
        //删除已创建和已完成但未销毁的任务
        if(ti->second.flgs&STask::FinishTask && !(ti->second.flgs&STask::Detached))
        {
            pthread_join(ti->second.thr, NULL);
            mTasks.erase(ti);
            continue;
        }

        res.release(); /*释放锁*/

        //当前活动任务出错
        if(time(NULL) >= (c_tm+wtm))
            return ;

        usleep(10 * 1000); /*10ms*/

        res.request(true);/*再加锁，进行下一次轮询*/
    }

    STask &htsk = mTasks[path];
    htsk.path = path;
    htsk.task = start_routine;
    htsk.taskArg = arg;
    htsk.flgs = 0;
    htsk.thr = 0;
    htsk.prior = priority%100; /*0~99*/

    res.release();

    if(pAttr)
    {
        pthr_attr = pAttr;
    }
    else
    {
        pthr_attr = &locPAttr;
        pthread_attr_init(pthr_attr); /*默认属性*/
    }

    /*新的线程继承策略和参数来自于schedpolicy和schedparam属性中显式设置的调度信息*/
    pthread_attr_setinheritsched(pthr_attr, PTHREAD_EXPLICIT_SCHED);

    struct sched_param prior;
    prior.sched_priority = 0;

    int policy = SCHED_OTHER; /*默认分时调度*/

    if(priority > 0)	policy = SCHED_RR; /*时间片*/
    if(priority >= 100)	policy = SCHED_FIFO; /*实时*/
    pthread_attr_setschedpolicy(pthr_attr, policy);

    prior.sched_priority = max(sched_get_priority_min(policy), min(sched_get_priority_max(policy),priority%100));

    pthread_attr_setschedparam(pthr_attr, &prior);//优先级设定

    try {
        pthread_attr_getdetachstate(pthr_attr,&detachStat);

        if(detachStat == PTHREAD_CREATE_DETACHED)
            htsk.flgs |= STask::Detached; /*判断是否线程分离*/

        int rez = pthread_create(&procPthr, pthr_attr, taskWrap, &htsk);

        if(rez == EPERM) /*操作不允许*/
        {
            policy = SCHED_OTHER;
            pthread_attr_setschedpolicy(pthr_attr, policy);
            prior.sched_priority = 0;
            pthread_attr_setschedparam(pthr_attr,&prior);

            rez = pthread_create(&procPthr, pthr_attr, taskWrap, &htsk);
        }

        if(!pAttr)
            pthread_attr_destroy(pthr_attr);

        if(rez) throw "Task creation error";

        //等待线程结构初始化完成，以完成不可分离的任务
        while(!(htsk.flgs&STask::Detached) && !htsk.thr)
            usleep(30 * 1000);

        //等待启动状态
        for(time_t c_tm = time(NULL); !(htsk.flgs&STask::Detached) && startSt && !(*startSt); )
        {
            if(time(NULL) >= (c_tm+wtm))
                throw "Task start timeouted!";

            usleep(100 * 1000);
        }
    } catch(const char *msg) {
        if(1) {		//删除pthread_create()的信息，但在稍后可能开始时将其留给其他函数
            res.request(true);
            mTasks.erase(path);
            res.release();
        }
        throw;
    }
}

void Task::taskDestroy(const string &path, bool *endrunCntr, int wtm)
{
    map<string,STask>::iterator it;

    ResAlloc res(taskRes, false); /*析构时自动释放销毁锁*/

    if(mTasks.find(path) == mTasks.end())
        return;

    res.release();/*释放锁*/

    if(endrunCntr)
        *endrunCntr = true; /*在这已经可以结束线程了*/

    //等待任务结束
    time_t t_tm, s_tm;
    t_tm = s_tm = time(NULL);


    res.request(true); /*写锁*/

    while((it=mTasks.find(path)) != mTasks.end() && !(it->second.flgs&STask::FinishTask))
    {
        res.release();

        time_t c_tm = time(NULL);

        //查看是否超时
        if(wtm && (c_tm > (s_tm+wtm))) {
            return ;
        }

        //
        if(c_tm > t_tm+1)
        {	//1sec
            t_tm = c_tm;
        }
        usleep(100 * 1000);/*100ms*/

        res.request(true);
    }
    if(it != mTasks.end()) {
        if(!(it->second.flgs&STask::Detached)) pthread_join(it->second.thr, NULL);
        mTasks.erase(it);
    }
}

void *taskWrap(void *stas)
{

    STask *tsk = (STask *)stas;


    void *(*wTask) (void *) = tsk->task;
    void *wTaskArg = tsk->taskArg;


    int policy;
    struct sched_param param;
    pthread_getschedparam(pthread_self(), &policy, &param);
    tsk->policy = policy;
    //tsk->prior = param.sched_priority;


    if(tsk->policy != SCHED_RR && tsk->prior > 0 && setpriority(PRIO_PROCESS,tsk->tid,-tsk->prior/5) != 0) tsk->prior = 0;
    tsk->thr = pthread_self();


    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    pthread_sigmask(SIG_BLOCK, &mask, NULL);


    void *rez = NULL;
    rez = wTask(wTaskArg);

    tsk->flgs |= STask::FinishTask;

    //> Remove task object for detached
    if(tsk->flgs & STask::Detached)	Task::taskDestroy(tsk->path, NULL);

    return rez;
}
