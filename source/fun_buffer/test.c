#include <stdio.h>
#include "message_queue.h"
struct message_queue mq;

int pack(unsigned char *input,const char *str)
{
    if(input && str)
    {
        printf("str = %s\n",str);
        memcpy(input,str,strlen(str) + 1);
        return strlen(str) + 1;
    }
    return -1;
}

int test_pack()
{
    int rv;
    struct message_item *item;
    mq_try_pack(mq,rv,item,pack(mq.curr,"test_pack"));

    mq_try_pack(mq,rv,item,pack(mq.curr,"xxxxx"));

    mq_try_pack(mq,rv,item,pack(mq.curr,"222"));
}

bool check(struct message_item *item,void *flag)
{
    if(item && flag)
    {
        printf("item = %s flag = %s\n",item->start,(char *)flag);
    }
    if(strcmp(item->start,(char *)flag) == 0)
        return true;
    return false;
}

int main(int argc,char *argv[])
{
    //struct message_queue mq;
    unsigned char buffer[1024] = "";

    mq_init(&mq,buffer,sizeof(buffer));

    test_pack();
    struct message_item *item;
    item = mq_find(&mq,check,"222");
    if(item)
    {
        printf("item = %s\n",item->start);
    }

    return 0;
}
