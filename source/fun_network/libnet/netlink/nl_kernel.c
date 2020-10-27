
/**
 *
 * ① 创建 netlink socket
 * net:
 * unit: netlink类型 NETLINK_TEST NETLINK_SELINUX
 * groups: 多播地址
 * input：netlink消息处理函数。当收到数据时调用
 * cb_mutex:访问数据时的互斥信号量
 * module: THIS_MODULE
 * struct sock * netlink_kernel_create(struct net *net,
 *          int unit,unsigned int groups,
 *          void (*input)(struct sk_buff *skb),
 *          struct mutex *cb_mutex,struct module *module);
 *
 *
 * ② 发送单播消息
 * int netlink_unicast(struct sock *ssk,struct sk_buff *skb,u32 id,int nonblock);
 *
 * skb: 存放消息的data数据  NETLINK_CB(skb)
 * pid:接收该消息的进程pid  如果是kernel接收=0
 * nonblock：1设置为非阻塞
 *
 * ③ 发送广播消息
 * int netlink_broadcast(struct sock *ssk,struct sk_buff *skb,u32 pid,u32 grup,gfp_t alloction);
 *
 *
 */
#include <linux/module.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>

#define NETLINK_USE     33
static struct sock *nl_sk = NULL;

static void msg_input(struct sk_buff *skb)
{
    struct nlmsghdr *nlh;
    int pid;
    struct sk_buff *skb_out;
    int msg_size;

    printk(KERN_INFO "msg input...%s \n",__FUNCTION__);

    msg_size = strlen(msg);
    nln = (struct nlmsghdr *)skb->data;
    printk(KERN_INFO "Netlink received msg : %s\n",(char*)nlmsg_data(nlh));

    pid = nlh->nlmsg_pid;

    sdk_out = nlmsg_new(msg_size,0);

    if(!skb_out)
    {
        /* failed to alloc */
        return ;
    }

    nlh = nlmsg_put(skb_out,0,0,NLMSG_DONE,msg_size,0);

    NETLINK_CB(skb_out).dst_group = 0;
    strncpy(nlmsg_data(nlh),msg,msg_size);

    res = nlmsg_unicast(nl_sk,skb_out,pid);
    if(res < 0)
        printk("....");
}

static int __init template_init(void)
{
    printk("template.... %s\n",__FUNCTION__);
    struct net kernel_net;
    nl_sk = netlink_kernel_create(&kernel_net,NETLINK_USER,msg_input,NULL,THIS_MODULE);
    if(!nl_sk)
    {
        return -1;
    }
    return 0;
};
module_init(template_init);

static void __exit template_exit(void)
{
    printk("....");
    netlink_kernel_release(nl_sk);
}
module_exit(template_exit);

/*
 *
    ifneq ($(KERNELRELEASE),)
    obj-m :=netlink_template.o
    else
    KERNELDIR ?=/lib/modules/$(shell uname -r)/build
    PWD :=$(shell pwd)
    default:
    $(MAKE) -C $(KERNELDIR) M=$(PWD) modules
    #endif
 *
 *
 */
