
#define TASK_RUNNING                    0
#define TASK_INTERRUPTIBLE              1
#define TASK_UNINTERRUPTIBLE            2
#define __TASK_STOPPED                  4
#define __TASK_TRACED                   8
/* Used in tsk->exit_state: */
#define EXIT_DEAD                       16
#define EXIT_ZOMBIE                     32
#define EXIT_TRACE                      (EXIT_ZOMBIE | EXIT_DEAD)
/* Used in tsk->state again: */
#define TASK_DEAD                       64
#define TASK_WAKEKILL                   128
#define TASK_WAKING                     256
#define TASK_PARKED                     512
#define TASK_NOLOAD                     1024
#define TASK_NEW                        2048
#define TASK_STATE_MAX                  4096
 
#define TASK_STATE_TO_CHAR_STR          "RSDTtXZxKWPNn"
 
/* Convenience macros for the sake of set_current_state: */
#define TASK_KILLABLE                   (TASK_WAKEKILL | TASK_UNINTERRUPTIBLE)
#define TASK_STOPPED                    (TASK_WAKEKILL | __TASK_STOPPED)
#define TASK_TRACED                     (TASK_WAKEKILL | __TASK_TRACED)
 
#define TASK_IDLE                       (TASK_UNINTERRUPTIBLE | TASK_NOLOAD)
 
/* Convenience macros for the sake of wake_up(): */
#define TASK_NORMAL                     (TASK_INTERRUPTIBLE | TASK_UNINTERRUPTIBLE)
#define TASK_ALL                        (TASK_NORMAL | __TASK_STOPPED | __TASK_TRACED)

/********************************************************************************************/
#include<linux/wait.h>
#include<linux/kthread.h>
#include<linux/list.h>
#include<linux/sched.h>
#include<linux/delay.h>
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("viz");
 
static int  __init __weakup_test_init(void);
static void __exit __weakup_test_exit(void);
 
static struct task_struct* old_task;
static struct wait_queue_head head;
 
int func_weakup(void* args)
{
   printk("func_weakup...\n");
   printk("this thread pid = %\d\n",current->pid);
 
   //print init status
   printk("init status, old_task->state = %ld\n",old_task->state);
 
   //__wake_up
   //__wake_up(&head,TASK_ALL,0,NULL);
 
   printk("after __weak_up, old_task->state = %ld\n",old_task->state);
return 0;
}
 
static int __init __weakup_test_init(void)
{
   printk("test init...\n");
 
   char namefrt[] = "__weakup.c:%s";
   long timeout;
   struct task_struct* result;
   struct wait_queue_entry data;
   printk("into weak up...\n");
   result = kthread_create_on_node(func_weakup,NULL,-1,namefrt);
   printk("the new thread pid is : %ld\n",result->pid);
   printk("the current pid is: %ld\n",current->pid);
   init_waitqueue_head(&head);
   init_waitqueue_entry(&data,current);
   add_wait_queue(&head,&data);
   old_task = current;
   //wake_up(&head);
    wake_up_process(result);
   timeout = schedule_timeout_uninterruptible(1000*10);
   printk("sleep timeout = %ld\n",timeout);
   printk("out __weakup_test_init\n");
return 0;
}
 
static void __exit __weakup_test_exit(void)
{
   printk("test exit...\n");
}
 
module_init(__weakup_test_init);
module_exit(__weakup_test_exit);