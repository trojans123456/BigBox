#include <sys/msg.h> /* fro msg queue */

int create_msg()
{
    /**
     msgflag：
        IPC_CREAT:创建新的消息队列。
        IPC_EXCL:与IPC_CREAT一同使用，表示如果要创建的消息队列已经存在，则返回错误。
        IPC_NOWAIT:读写消息队列要求无法满足时，不阻塞。返回值： 调用成功返回队列标识符,否则返回-1.
     */
    int ret = msgget(IPC_PRIVATE,IPC_CREAT);
    if(ret < 0)
        return -1;
    return ret; /* return msg id */
}

int ctrl_msg(int msgid,int cmd)
{
    /**
        IPC_STAT get msqid_ds into buf
        IPC_SET modify content buf into kernel msqid_ds with this msg queue
        IPC_RMID del msgid
    */
    struct msqid_ds buf;
    return msgctl(msgid,cmd,&buf);
}

struct my_msg
{
    long type;
    char text[512];
};

int send_msg(int msgid,int id,char *text,int len)
{
    struct my_msg msg;
    msg.type = id;
    memcpy(msg.text,text,len);
    int ret = msgsnd(msgid,buf,len,0);
    return ret;
}

int recv_msg(int msgid,int id,char *text,int *len)
{
    /**
        type(id) == 0 return first msg from queue
                 > 0 return this msg with type
                 < 0 return less id msg
    */
    ssize_t ret = msgrcv(msgid,text,*len,id,0);
    return ret;
}
