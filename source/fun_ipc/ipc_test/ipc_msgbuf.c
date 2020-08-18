#include <stdio.h>

#ifndef __WIN32__
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#endif

/**
*1.创建标识码
* key_t ftok(const char *pathname,int proj_id);
*/

/**
*2.创建和访问消息队列
* msgflag = IPC_CREATE IPC_EXCL
* int msgget(key_t key,int msgflag);
*/

/**
*3.消息队列的控制
* cmd =
* IPC_STAT 把msqid_ds结构中的数据设置为消息队列的当前关联值
* IPC_SET 在进程有足够权限的前提下，把消息队列的当前关联值设置为msqid_ds数据结构中给出的值
* IPC_RMID 删除消息队列
*int msgctl(int msqid,int cmd,struct msgid_ds *buf);
*/

/**
*4. 发送一条消息
* int msgsnd(int msgid,const void *msgq,size_t msgsz,int msgflag);
*/

/**
*5. 接收消息
* int msgrcv(int msgid,void *msgp,size_t msgsz,long msgtyp,int msgflag);
*/

/** ipcs 显示ipc资源 ipcrm 删除ipc资源 */

struct msgbuf
{
    long mtype;
    char mtext[1024];
};

int msg_init(void)
{
    key_t key = ftok("/tmp",0x6666);
    if(key < 0)
    {
        return -1;
    }

    int msg_id = msgget(key,IPC_CREAT | IPC_EXCL | 0666);
    if(msg_id < 0)
    {
        return -1;
    }
    return msg_id;
}

int msg_destroy(int msg_id)
{
    if(msgctl(msg_id,IPC_RMID,NULL) < 0)
    {
        return -1;
    }
    return 0;
}

int msg_send(int msg_id,int who,char *msg)
{
    struct msgbuf buf;
    buf.mtype = who;
    strcpy(buf.mtext,msg);

    if(msgsnd(msg_id,(void*)&buf,sizeof(buf.mtext),0) < 0)
    {
        return -1;
    }
    return 0;
}

int msg_recv(int msg_id,int recvType,char *out)
{
    struct msgbuf buf;
    int size = sizeof(buf.mtext);

    if(msgrcv(msg_id,(void *)&buf,size,recvType,0) < 0)
    {
        return -1;
    }

    strncpy(out,buf.mtext.size);
    out[size] = '\0';

    return 0;
}

#define CLIENT_TYPE 1
#define SERVER_TYPE 2

int server_main()
{
    int msg_id = msg_init();
    char buf[1024] = "";

    while(1)
    {
        msg_recv(msg_id,CLIENT_TYPE,buf);
        printf("buf = %s\n",buf);
    }
}

int client_main()
{
    int msg_id = msg_init();
    char buf[1024] = "";

    while(1)
    {
        msg_send(msg_id,SERVER_TYPE,"xxx");
        sleep(1);
    }
}
