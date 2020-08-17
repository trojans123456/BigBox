#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>

/**
 *int shmget(key_t key,size_t size,int shmflg); //shmget 函数用来创建一个
新的共享内存段， 或者访问一个现有的共享内存段（不同进程只要 key 值相同即可访问
同一共享内存段） 。 第一个参数 key 是 ftok 生成的键值， 第二个参数 size 为共享内存的
大小， 第三个参数 sem_flags 是打开共享内存的方式。


2.eg.int shmid = shmget(key, 1024, IPC_CREATE | IPC_EXCL | 0666);//第三个
参数参考消息队列 int msgget(key_t key,int msgflag);


3. void *shmat(int shm_id,const void *shm_addr,int shmflg); //shmat 函数
通过 shm_id 将共享内存连接到进程的地址空间中。 第二个参数可以由用户指定共享内存
映射到进程空间的地址， shm_addr 如果为 0， 则由内核试着查找一个未映射的区域。 返回
值为共享内存映射的地址。


4. eg.char *shms = (char *)shmat(shmid, 0, 0);//shmid 由 shmget 获得

5. int shmdt(const void *shm_addr); //shmdt 函数将共享内存从当前进程中分
离。 参数为共享内存映射的地址。
6. eg.shmdt(shms);

7. int shmctl(int shm_id,int cmd,struct shmid_ds *buf);//shmctl 函数是控制
函数， 使用方法和消息队列 msgctl()函数调用完全类似。 参数一 shm_id 是共享内存的句
柄， cmd 是向共享内存发送的命令， 最后一个参数 buf 是向共享内存发送命令的参数
 *
 *
*/
