#ifndef __THREAD_H
#define __THREAD_H

#ifndef __WIN32__
#include <pthread.h>
#endif

#include <string>

using namespace std;

namespace ns_core {


class Thread
{
public:
    Thread();
    virtual ~Thread();
    enum ThreadStatus
    {
        THREAD_STATUS_NEW = 0,//新建
        THREAD_STATUS_RUNNING = 1,//运行
        THREAD_STATUS_EXIT = -1 //结束
    };

    static int thread_detach(pthread_t pid);
    static int thread_join(pthread_t pid,void **retVal);
    static int thread_cancel(pthread_t pid);

    virtual void run() = 0;//运行实体

    bool start();//启动
    int  cancle();//取消
    void stop();
    int yield();

    int kill();
    void join();
    void join(unsigned long ms);//等待线程退出或超时
    pthread_t getThreadId();//获取线程id
    int getStatus();
    void threadSleep(unsigned long ms);

private:
    pthread_t tid;
    int threadStatus;
    //线程回调指针
    static void *run0(void *pVoid);
    void *run1();//内部执行方法
};

}



#endif
