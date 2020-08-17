/**
* linux信号量分为systemv(semget) posix (sem_int)两种
*/

/** systemv */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

/**
*1.创建信号量
* flags = IPC_CREAT | IPC_EXCL |0666
* int semget(key_t key,int nsem,int flags);
*/

/**
*2.删除和控制信号量
* semnum = 当前信号量级的哪一个
* cmd = IPC_STAT IPC_SET IPC_RMID
* int semctl(int semid,int semnum,int cmd,...);
* 可选参数
* union semun
{
    int val;  //使用的值
    struct semid_ds *buf;  //IPC_STAT、IPC_SET 使用的缓存区
    unsigned short *arry;  //GETALL,、SETALL 使用的数组
    struct seminfo *__buf; // IPC_INFO(Linux特有) 使用的缓存区
};
*/

/**
*3.改变信号量值
* int semop(int semid,struct sembuf *sops,size_t nops);
* struct sembuf{
    short sem_num;   //除非使用一组信号量，否则它为0
    short sem_op;   //信号量在一次操作中需要改变的数据，通常是两个数，
                    //一个是-1，即P（等待）操作，
                    //一个是+1，即V（发送信号）操作。
    short sem_flg; //通常为SEM_UNDO,使操作系统跟踪信号量，
                  //并在进程没有释放该信号量而终止时，操作系统释放信号量
};
*/


/**** posix 信号量 ***/
#include <semaphore.h>

/**
*1.创建
* pshared 0线程 1进程
* int sem_init(sem_t *sem,int pshared,unsigned int value);
*/

/**
*2.销毁
* int sem_destroy(sem_t *sem);
*/

/**
* int sem_wait(sem_t *sem);
*
* int sem_trywait(sem_t *sem);
*
* int sem_timedwait(sem_t *sem,const struct timespec *abs_timeout);
*
* int sem_post(sem_t *sem);
*
* int sem_getvalue(sem_t *restrict,int *restrict);
*/
