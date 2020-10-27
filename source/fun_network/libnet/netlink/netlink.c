#ifdef __linux__
#include <linux/netlink.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "netlink.h"

/*
 *
 */

int create_netlink(int type)
{
    int sockfd = -1;
    struct sockaddr_nl src_addr;
    /* SOCK_RAW or SOCK_DGRAM 面向服务的*/
    sockfd = socket(PF_NETLINK,SOCK_RAW,type);
    if(sockfd < 0)
        return -1;

    memset(&src_addr,0,sizeof(src_addr));

    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid();

    bind(sockfd,(struct sockaddr *)&src_addr,sizeof(src_addr));

    return sockfd;
}

int sendall(int sockfd, void *buf, int buffer_len)
{
    struct sockaddr_nl dest_addr;
    struct nlmsghdr *nlh = NULL;
    struct iovec iov;
    struct msghdr msg;

    memset(&dest_addr,0,sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0;
    dest_addr.nl_groups = 0; /* 单播 */

    nlh = (struct nlmsghdr *)calloc(1,NLMSG_SPACE(buffer_len));
    if(!nlh)
        return -1;

    nlh->nlmsg_len = NLMSG_SPACE(buffer_len);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;

    memcpy(NLMSG_DATA(nlh),buf,buffer_len);

    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;

    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    return sendmsg(sockfd,&msg,0);
}

int recvall(int sockfd, void *buf, int buffer_len)
{
    struct sockaddr_nl dest_addr;
    struct nlmsghdr *nlh = NULL;
    struct iovec iov;
    struct msghdr msg;

    memset(&dest_addr,0,sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0;
    dest_addr.nl_groups = 0; /* 单播 */

    nlh = (struct nlmsghdr *)calloc(1,NLMSG_SPACE(buffer_len));
    if(!nlh)
        return -1;

    nlh->nlmsg_len = NLMSG_SPACE(buffer_len);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;


    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;

    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    ssize_t len = recvmsg(sockfd,&msg,0);

    memcpy(buf,NLMSG_DATA(nlh),len > buffer_len ? buffer_len : len);

    return len;
}
