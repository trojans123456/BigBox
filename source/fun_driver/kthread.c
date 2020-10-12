

#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>

static struct task_struct *kThread = NULL;

#define sleep_sec(msec) \
do{\
    long timeout = (msec) * HZ / 1000;\
    while(timeout > 0) \
    {\
        timeout = schedule_timeout(timeout); \
    }\
}while(0)

static int kThreadFun(void *data)
{
    char *mydata = kzalloc(strlen(data) + 1,GFP_KERNEL);
    strncpy(mydata,data,strlen(data));
    while(!kthread_should_stop())
    {
        sleep_sec(1000);
        printk("%s\n",mydata);
    }
    // msleep_interruptible(timeout); maybe interrupt by signal
    kfree(mydata);
    return 0;
}

static int __init init_kthread(void)
{
    kThread = kthread_run(kThreadFun,"test","test_thread");
    return 0;
}

static void __exit exit_kthread(void)
{
    if(kThread)
    {
        kthread_stop(kThread);
    }
}

module_init(init_kthread);
module_exit(exit_kthread);

MODULE_LICENSE("GPL");



