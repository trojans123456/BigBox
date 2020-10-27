#ifdef __linux__
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h> /* for getadddrinfo */
#include <unistd.h>
#include <fcntl.h>
#include <sys/un.h> /* for unix */
#include <errno.h>
#include <netinet/in.h> /* for inet_ntoa */
#include <arpa/inet.h> /* for inet_ntoa*/
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <net/if_arp.h>
#include <net/if.h>
#include <netinet/tcp.h> /* for keepalive...*/
#endif
#include <stdio.h>
#include <string.h>

#include "net_socket.h"

int create_tcp_socket(const char *hostname, const char *service, unsigned char proto)
{
    int sockfd = -1;
    int ret = -1;
    const char *err_str;
    struct addrinfo hint,*result,*addrinfo;

    if(!hostname || !service)
        return -1;

    memset(&hint,0,sizeof(hint));

    switch(proto)
    {
    case IP_TYPE_V4:
        hint.ai_family = AF_INET;
        break;
    case IP_TYPE_V6:
        hint.ai_family = AF_INET6;
        break;
    case IP_TYPE_BOTH:
        hint.ai_family = AF_UNSPEC;
        break;
    default:
        return -1;
    }

    hint.ai_socktype = SOCK_STREAM; // tcp

    if((ret = getaddrinfo(hostname,service,&hint,&result)) != 0)
    {
        err_str = gai_strerror(ret);
        printf("%s \n",err_str);
        return -1;
    }

    for(addrinfo = result;addrinfo != NULL;addrinfo = addrinfo->ai_next)
    {
        sockfd = socket(addrinfo->ai_family,addrinfo->ai_socktype,addrinfo->ai_protocol);
        if(sockfd < 0)
            continue;

        if(connect(sockfd,addrinfo->ai_addr,addrinfo->ai_addrlen) != -1)
            break;

        close(sockfd);
    }

    if(!addrinfo)
    {
        close(sockfd);
        freeaddrinfo(result);
        return -1;
    }

    freeaddrinfo(result);

    return sockfd;
}

int create_udp_socket(unsigned char proto)
{
    int sockfd = -1;
    if(proto != IP_TYPE_V4 || proto != IP_TYPE_V6)
        return -1;

    switch(proto)
    {
    case IP_TYPE_V4:
        sockfd = socket(AF_INET,SOCK_DGRAM,0);
        break;
    case IP_TYPE_V6:
        sockfd = socket(AF_INET6,SOCK_DGRAM,0);
        break;
    default:
        return -1;
    }

    if(sockfd < 0)
        return -1;

    return sockfd;
}

int create_unix_socket(const char *path)
{
    struct sockaddr_un saddr;
    int sfd;

    if (path == NULL) return -1;


    if (-1 == (sfd = socket(AF_UNIX, SOCK_STREAM, 0)))
        return -1;

    memset(&saddr, 0, sizeof(struct sockaddr_un));

    if (strlen(path) > (sizeof(saddr.sun_path) - 1)) {
        return -1;
    }

    saddr.sun_family = AF_UNIX;
    strncpy(saddr.sun_path, path, sizeof(saddr.sun_path) - 1);

    if (-1 == connect(sfd, (struct sockaddr*)&saddr,sizeof(saddr.sun_family) + strlen(saddr.sun_path))) {
        close(sfd);
        return -1;
    }

    return sfd;
}

/**
* struct addrinfo
* {
*   int        　　　　    　　ai_flags;//指示在getaddrinfo函数中使用的选项的标志。
*   int        　　　　   　　 ai_family;
*   int        　　　　    　　ai_socktype;
*   int        　　　　　 　　ai_protocol;
*   size_t       　　　　 　　ai_addrlen;
*   char       　　　　 　　*ai_canonname;
*   struct sockaddr    　　*ai_addr;
*   struct addrinfo     　　*ai_next;
* };
*
* struct sockaddr
* {
*   ushort  sa_family;
*   char    sa_data[14];
* };
* struct sockaddr_in
* {
*   short           sin_family; //指代协议族，在socket编程中只能是 AF_INET
*   u_short         sin_port;   //存储端口号（使用网络字节顺序）
*   struct in_addr  sin_addr;   //存储IP地址，使用in_addr这个数据结构
*   char            sin_zero[8];//为了让sockaddr与sockaddr_in两个数据结构保持大小相同而保留的空字节。
* };
*
*/

int create_unix_server(const char *path,int listen_max)
{
    struct sockaddr_un saddr;
    int sfd, type, retval;

    if (path == NULL || listen_max <= 0) return -1;

    if (strlen(path) > (sizeof(saddr.sun_path) - 1)) {
        return -1;
    }

    type = SOCK_STREAM;

    if (-1 == (sfd = socket(AF_UNIX, type, 0))) return -1;

    if ((retval = unlink(path)) == -1 &&
        errno != ENOENT)  // If there's another error than "doesn't exist"
    {
        return -1;
    }

    memset(&saddr, 0, sizeof(struct sockaddr_un));

    saddr.sun_family = AF_UNIX;

    strncpy(saddr.sun_path, path, sizeof(saddr.sun_path) - 1);

    if (-1 ==bind(sfd, (struct sockaddr*)&saddr,sizeof(saddr.sun_family) + strlen(saddr.sun_path)))
        return -1;

    if (type == SOCK_STREAM) {
        if (-1 == listen(sfd, listen_max)) return -1;
    }

    return sfd;
}

int udp_connect_to(int sockfd,const char *host, const char *service)
{
    struct addrinfo *result,*addrinfo,hint;
    struct sockaddr_storage oldsockaddr;
    struct sockaddr deconnect;

    socklen_t oldsockaddrlen = sizeof(struct sockaddr_storage);
    int return_value;
    const char *err_str;

    if(sockfd < 0)
        return -1;

    if(host == NULL)
    {
        memset(&deconnect,0,sizeof(struct sockaddr));
        deconnect.sa_family = AF_UNSPEC;
        if(connect(sockfd,&deconnect,sizeof(struct sockaddr)) < 0)
            return -1;

        return 0;
    }

    if(getsockname(sockfd,(struct sockaddr *)&oldsockaddr,
                   &oldsockaddrlen) < 0)
    {
        return -1;
    }
    if(oldsockaddrlen > sizeof(struct sockaddr_storage))
        return -1;

    memset(&hint,0,sizeof(struct addrinfo));
    hint.ai_family = ((struct sockaddr_in *)&oldsockaddr)
                         ->sin_family;  // AF_INET or AF_INET6 - offset is same
                                        // at sockaddr_in and sockaddr_in6
    hint.ai_socktype = SOCK_DGRAM;

    if((return_value = getaddrinfo(host,service,&hint,&result)) < 0)
    {
        err_str = gai_strerror(return_value);
        printf("%s\n",err_str);
        return -1;
    }

    for (addrinfo = result; addrinfo != NULL;
         addrinfo = addrinfo->ai_next)
    {
        if (-1 != (return_value = connect(
                       sockfd, addrinfo->ai_addr,
                       addrinfo->ai_addrlen)))  // connected without error
        {
            break;
        } else {
            return -1;
        }
    }


    if(addrinfo == NULL)
    {
        freeaddrinfo(result);
        return -1;
    }

    freeaddrinfo(result);
    return 0;
}


ssize_t udp_sendto(int sockfd, void *buf, size_t buf_len, const char *host, const char *service)
{
    struct sockaddr_storage oldsock;
    struct addrinfo hint,*result,*addrinfo;
    socklen_t oldsocklen = sizeof(struct sockaddr_storage);
    int ret;
    const char *err_str = NULL;

    if(sockfd < 0 || !buf || buf_len == 0 || !host || !service)
        return -1;

    /* 通过sockfd 获取相关地址*/
    if(getsockname(sockfd,(struct sockaddr *)&oldsock,&oldsocklen) < 0)
        return -1;

    memset(&hint,0,sizeof(struct addrinfo));

    hint.ai_family = oldsock.ss_family;
    hint.ai_socktype = SOCK_DGRAM;

    if((ret = getaddrinfo(host,service,&hint,&result)) < 0)
    {
        err_str = gai_strerror(ret);
        printf("%s \n",err_str);
        return -1;
    }

    for(addrinfo = result;addrinfo != NULL;addrinfo = addrinfo->ai_next)
    {
        if((ret = sendto(sockfd,buf,buf_len,0,addrinfo->ai_addr,addrinfo->ai_addrlen)) != -1)
        {
            break;
        }
        else
        {
            return -1;
        }
    }

    freeaddrinfo(result);

    return ret;
}

ssize_t udp_recvfrom(int sockfd,void *buf,size_t buf_len,
                     char *src_host,size_t src_host_len,
                     char *src_service,size_t src_service_len)
{
    struct sockaddr_storage client;

    /* for getnameinfo 通过sockfd 反向解析主机名和端口(服务) */
    struct sockaddr_storage oldsockaddr;
    socklen_t oldsockaddrlen = sizeof(struct sockaddr_storage);
    struct hostent *he = NULL;
    void *addrptr = NULL;
    size_t addrlen = 0;
    uint16_t sport = 0;


    ssize_t bytes = 0;

    if(sockfd < 0)
        return -1;

    if(!buf || buf_len == 0)
        return -1;

    memset(buf,0,buf_len);

    if(src_host)    memset(src_host,0,src_host_len);
    if(src_service) memset(src_service,0,src_service_len);

    socklen_t stor_addrlen = sizeof(struct sockaddr_storage);

    if((bytes == recvfrom(sockfd,buf,buf_len,0,(struct sockaddr *)&client,&stor_addrlen)) < 0)
    {
        return -1;
    }
#if 0
    int retval;
    int numeric = NI_NUMERICHOST | NI_NUMERICSERV;
    if((retval = getnameinfo((struct sockaddr *)&client,sizeof(struct sockaddr_storage),
                             src_host, src_host_len, src_service, src_service_len,
                             numeric)) != 0)
    {
        return -1;
    }
#endif

    if (getsockname(sockfd, (struct sockaddr *)&oldsockaddr,
                                      &oldsockaddrlen) < 0)
    {
        return -1;
    }


    if (oldsockaddrlen >
        sizeof(struct sockaddr_storage))  {return -1;}

    if (oldsockaddr.ss_family == AF_INET) {
        addrptr = &(((struct sockaddr_in *)&client)->sin_addr);
        addrlen = sizeof(struct in_addr);
        sport = ntohs(((struct sockaddr_in *)&client)->sin_port);
    } else if (oldsockaddr.ss_family == AF_INET6) {
        addrptr = &(((struct sockaddr_in6 *)&client)->sin6_addr);
        addrlen = sizeof(struct in6_addr);
        sport = ntohs(((struct sockaddr_in6 *)&client)->sin6_port);
    }

    if (NULL ==(he = gethostbyaddr(addrptr, addrlen, oldsockaddr.ss_family))) {

        return -1;
    }

    strncpy(src_host, he->h_name, src_host_len);
    snprintf(src_service, src_service_len, "%u", sport);

    return bytes;
}

int close_socket(int sockfd)
{
    if(sockfd < 0)
        return -1;

    close(sockfd);
    return 0;
}

int shutdown_socket(int sockfd, int flags)
{
    if(sockfd < 0)
        return -1;
    if((flags != SHUTDOWN_TYPE_READ) &&
       (flags != SHUTDOWN_TYPE_WRITE) &&
       (flags != (SHUTDOWN_TYPE_READ | SHUTDOWN_TYPE_WRITE)))
    {
        return -1;
    }

    if(flags & SHUTDOWN_TYPE_READ)
    {
        shutdown(sockfd,SHUT_RD);
    }
    if(flags & SHUTDOWN_TYPE_WRITE)
    {
        shutdown(sockfd,SHUT_WR);
    }
    return 0;
}

int set_nonblock(int sockfd)
{
    int flags = 0;
    flags = fcntl(sockfd,F_GETFL,0);
    if(flags == -1)
        return -1;

    if(fcntl(sockfd,F_SETFL,flags | O_NONBLOCK) < 0)
        return -1;
    return 1;
}

int set_block(int sockfd)
{
    int flags = 0;
    flags = fcntl(sockfd,F_GETFL,0);
    if(flags == -1)
        return -1;

    if(fcntl(sockfd,F_SETFL,flags & ~O_NONBLOCK) < 0)
        return -1;
    return 1;
}

int get_ip_family(const char *hostname)
{
    struct addrinfo hint,*result;

    int af = -1;
    if(!hostname)
        return -1;

    memset(&hint,0,sizeof(hint));
    hint.ai_family = AF_UNSPEC;

    if(getaddrinfo(hostname,"0",&hint,&result) < 0)
        return -1;

    if(result == NULL)
        return -1;

    if(result->ai_family == AF_INET)
    {
        af = IP_TYPE_V4;
    }
    else if(result->ai_family == AF_INET6)
    {
        af = IP_TYPE_V6;
    }
    return af;
}

int create_server_socket(const char *bind_addr, const char *bind_port, unsigned char sock_type, unsigned char ip_type, int listen_max)
{
    int sockfd = -1,type,domain;
    int ret = -1;
    struct addrinfo hint,*result,*addrinfo;

    if(!bind_addr || !bind_port || listen_max <= 0)
        return -1;
    switch(sock_type)
    {
    case SOCKET_TYPE_TCP:
        type = SOCK_STREAM;
        break;
    case SOCKET_TYPE_UDP:
        type = SOCK_DGRAM;
        break;
    default:
        return -1;
    }
    switch(ip_type)
    {
    case IP_TYPE_V4:
        domain = AF_INET;
        break;
    case IP_TYPE_V6:
        domain = AF_INET6;
        break;
    case IP_TYPE_BOTH:
        domain = AF_UNSPEC;
        break;
    default:
        return -1;
    }

    memset(&hint,0,sizeof(struct addrinfo));

    hint.ai_socktype = type;
    hint.ai_family = domain;
    hint.ai_flags = AI_PASSIVE;

    if(getaddrinfo(bind_addr,bind_port,&hint,&result) < 0)
        return -1;

    for(addrinfo = result;addrinfo != NULL;addrinfo = addrinfo->ai_next)
    {
        sockfd = socket(addrinfo->ai_family,addrinfo->ai_socktype,addrinfo->ai_protocol);
        if(sockfd < 0)
            continue;

        set_opt_reuseaddr(sockfd);
        ret = bind(sockfd,addrinfo->ai_addr,addrinfo->ai_addrlen);
        if(ret != 0)
        {
            close(sockfd);
            continue;
        }
        printf("addr = %s\n",inet_ntoa(((struct sockaddr_in *)(addrinfo->ai_addr))->sin_addr));
        if(type == SOCKET_TYPE_TCP)
        {
            ret = listen(sockfd,listen_max);
        }
        if(ret == 0)
        {
            break;
        }
        else
        {
            close(sockfd);
        }
    }

    if(addrinfo == NULL)
    {
        freeaddrinfo(result);
        return -1;
    }

    freeaddrinfo(result);
    return sockfd;
}

int accept_socket(int sockfd, char *src_host, size_t src_host_len, char *src_service, size_t src_service_len)
{
    struct sockaddr_storage client_info;
    int client_fd;

    struct sockaddr_storage oldsockaddr;
    socklen_t oldsockaddrlen = sizeof(struct sockaddr_storage);
    struct hostent *he = NULL;
    void *addrptr = NULL;
    size_t in_addrlen = 0;
    uint16_t sport = 0;

    socklen_t addrlen = sizeof(struct sockaddr_storage);

    if((client_fd = accept(sockfd,(struct sockaddr *)&client_info,&addrlen)) < 0)
    {
        return -1;
    }

#if 0
    int retval;
    int flags = NI_NUMERICHOST | NI_NUMERICSERV;
    if (0 != (retval = getnameinfo(
                  (struct sockaddr *)&client_info,
                  sizeof(struct sockaddr_storage), src_host, src_host_len,
                  src_service, src_service_len,
                  flags)))  // Write information to the provided memory
    {
        return -1;
    }
#endif

    if(src_host_len <= 0 ||
       src_service_len <= 0 ||
       !src_host || !src_service)
    {
        return client_fd;
    }

    if (-1 == getsockname(sockfd, (struct sockaddr *)&oldsockaddr,
                                      &oldsockaddrlen))
        return -1;

    if (oldsockaddrlen >
        sizeof(struct sockaddr_storage))  // If getsockname truncated the
                                          // struct
        return -1;

    if (oldsockaddr.ss_family == AF_INET) {
        addrptr = &(((struct sockaddr_in *)&client_info)->sin_addr);
        in_addrlen = sizeof(struct in_addr);
        sport = ntohs(((struct sockaddr_in *)&client_info)->sin_port);
    } else if (oldsockaddr.ss_family == AF_INET6) {
        addrptr = &(((struct sockaddr_in6 *)&client_info)->sin6_addr);
        in_addrlen = sizeof(struct in6_addr);
        sport = ntohs(((struct sockaddr_in6 *)&client_info)->sin6_port);
    }

    if (NULL ==(he = gethostbyaddr(addrptr, in_addrlen, oldsockaddr.ss_family))) {

    }

    strncpy(src_host, he->h_name, src_host_len);
    snprintf(src_service, src_service_len, "%u", sport);

    return client_fd;
}

int set_opt_keepalive(int fd, int interval)
{
    int val = 1;

    if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &val, sizeof(val)) == -1)
    {
        return -1;
    }

    val = interval;
    /* 如果interval内没有任何数据交互,则进行探测,默认7200s*/
    if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, &val, sizeof(val)) < 0) {
        return -1;
    }

    val = interval/3;
    /* 探测时发探测包的时间间隔为 interval / 3 秒*/
    if (val == 0) val = 1;
    if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, &val, sizeof(val)) < 0) {
        return -1;
    }

    val = 3;
    /* 探测重试的次数 缺省9次*/
    if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT, &val, sizeof(val)) < 0) {
        return -1;
    }
    return 0;
}

int set_opt_nodelay(int sockfd, int ok)
{
    /*控制ngale算法是否开启*/
    if(setsockopt(sockfd,IPPROTO_TCP,TCP_NODELAY,&ok,sizeof(ok)) == -1)
        return -1;
    return 0;
}

int set_opt_sendbuffer(int fd, int buffsize)
{
    if (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &buffsize, sizeof(buffsize)) == -1)
    {
       return -1;
    }
    return 0;
}

int set_opt_sendtimeout(int fd, long long ms)
{
    struct timeval tv;

    tv.tv_sec = ms/1000;
    tv.tv_usec = (ms%1000)*1000;
    /*发送超时时间*/
    if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) == -1) {
        return -1;
    }
    return 0;
}

int set_opt_ipv6_only(int sockfd)
{
    int yes = 1;
    if (setsockopt(sockfd,IPPROTO_IPV6,IPV6_V6ONLY,&yes,sizeof(yes)) == -1) {

        //close(sockfd);
        return -1;
    }
    return 1;
}

int set_opt_reuseaddr(int sockfd)
{
    int mw_optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&mw_optval,sizeof(mw_optval));
}

int sendall(int sockfd, void *buf, size_t buf_len)
{
    int pos = 0;
    int real = 0;
    while(pos < buf_len)
    {
        real = send(sockfd,buf + pos,buf_len - pos,0);
        if(real < 0)
            return -1;
        if(real == 0)
            break;

        pos += real;
    }
    return pos;
}

int recvall(int sockfd, void *buf, size_t buf_len)
{
    int rv = 0;
    void *start = buf;
    do
    {
        rv = recv(sockfd,buf,buf_len,0);

        if(rv > 0)
        {
            buf += rv;
            buf_len -= rv;
        }
        else if(rv < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
        {
            return -1;
        }
        else if(rv == 0)
        {
            break;
        }
    }while(rv > 0 && buf_len > 0);

    return buf - start;
}

int readall(int sockfd, void *buf, int count)
{
    ssize_t nread,totlen = 0;

    while(totlen != count)
    {
        nread = read(sockfd,buf,count - totlen);
        if(nread == 0)
        {
            return totlen;
        }
        if(nread == -1)
        {
            return -1;
        }

        totlen += nread;
        buf += nread;
    }
    return totlen;
}

int writeall(int sockfd, void *buf, int count)
{
    ssize_t nwritten,totlen = 0;

    while(totlen != count)
    {
        nwritten = write(sockfd,buf,count - totlen);
        if(nwritten == 0)
        {
            return totlen;
        }
        if(nwritten == -1)
        {
            return -1;
        }

        totlen += nwritten;
        buf += nwritten;
    }
    return totlen;
}

int create_can_socket(const char *name)
{
    int fd;
    struct sockaddr_can addr;
    struct ifreq ifr;

    if((fd = socket(PF_CAN,SOCK_RAW,CAN_RAW)) < 0)
        return fd;

    //wish name
    strcpy(ifr.ifr_name,name);
    if((ioctl(fd,SIOCGIFINDEX,&ifr)) < 0) //Specify the name
    {
        return -1;
    }

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if((bind(fd,(struct sockaddr *)&addr,sizeof(addr))) < 0)
    {
        close(fd);
        return -1;
    }
    return fd;
}

/** 禁止接收过滤规则,kernel 会丢掉该fd收到的can帧*/
void disable_can_reciverfileter(int canfd)
{
    setsockopt(canfd,SOL_CAN_RAW,CAN_RAW_FILTER,NULL,0);
}

/** 禁止回环*/
void disable_can_loopback(int canfd)
{
    int loopback = 0;
    setsockopt(canfd,SOL_CAN_RAW,CAN_RAW_LOOPBACK,&loopback,sizeof(loopback));
}

void set_can_filter(int canfd,struct can_filter *filter,int count)
{
    setsockopt(canfd,SOL_CAN_RAW,CAN_RAW_FILTER,filter,count);
}

int get_local_ipaddr(const char *devname, char *result)
{
    if(!devname || !result)
        return -1;

    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd < 0)
        return -1;

    struct ifreq ifr;
    bzero(&ifr,sizeof(struct ifreq));
    strcpy(ifr.ifr_name,devname);

    if(ioctl(sockfd,SIOCGIFADDR,&ifr) < 0)
    {
        close(sockfd);
        return -1;
    }


    struct sockaddr_in *pAddr;
    pAddr = (struct sockaddr_in *)&(ifr.ifr_addr);

    char addr[64] = "";
    const char *ptr = inet_ntop(AF_INET,&(pAddr->sin_addr),addr,sizeof(addr));
    if(ptr == NULL)
    {
        close(sockfd);
        return -1;
    }

    close(sockfd);
    strncpy(result,addr,sizeof(addr));

    return 1;
}

int get_local_macaddr(const char *devname, char *result)
{
    if(!devname || !result)
        return -1;

    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd < 0)
        return -1;

    struct ifreq ifr;
    bzero(&ifr,sizeof(struct ifreq));
    strcpy(ifr.ifr_name,devname);

    if(ioctl(sockfd,SIOCGIFHWADDR,&ifr) < 0)
    {
        close(sockfd);
        return -1;
    }


    sprintf(result,"%02x:%02x:%02x:%02x:%02x:%02x",(unsigned char)ifr.ifr_hwaddr.sa_data[0],\
            (unsigned char)ifr.ifr_hwaddr.sa_data[1],\
            (unsigned char)ifr.ifr_hwaddr.sa_data[2],\
            (unsigned char)ifr.ifr_hwaddr.sa_data[3],\
            (unsigned char)ifr.ifr_hwaddr.sa_data[4],\
            (unsigned char)ifr.ifr_hwaddr.sa_data[5]);

    close(sockfd);
    return 1;
}

int get_local_nettype(const char *devname)
{
    if(!devname)
        return LOCAL_NET_TYPE_NONE;

    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd < 0)
        return LOCAL_NET_TYPE_NONE;

    struct ifreq ifr;
    bzero(&ifr,sizeof(struct ifreq));
    strcpy(ifr.ifr_name,devname);

    if(ioctl(sockfd,SIOCGIFHWADDR,&ifr) < 0)
    {
        close(sockfd);
        return LOCAL_NET_TYPE_NONE;
    }


    int ret = LOCAL_NET_TYPE_NONE;
    switch(ifr.ifr_hwaddr.sa_family)
    {
    case ARPHRD_ETHER:
        ret = LOCAL_NET_TYPE_ETH;
        break;
    case ARPHRD_PPP:
        ret = LOCAL_NET_TYPE_PPP;
        break;
    case ARPHRD_LOOPBACK:
        ret = LOCAL_NET_TYPE_LOOP;
        break;
    default:
        ret = LOCAL_NET_TYPE_NONE;
    }

    close(sockfd);
    return ret;
}

int get_local_mask(const char *devname, char *result)
{
    if(!devname || !result)
        return -1;

    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd < 0)
        return -1;

    struct sockaddr_in *pAddr;
    struct ifreq ifr;

    memset(&ifr,0,sizeof(ifr));
    strcpy(ifr.ifr_name,devname);

    if(ioctl(sockfd,SIOCGIFNETMASK,&ifr) < 0)
    {
        close(sockfd);
        return -1;
    }


    pAddr = (struct sockaddr_in *)&(ifr.ifr_addr);
    char buf[64] = "";
    const char *ptr = inet_ntop(AF_INET,&(pAddr->sin_addr),buf,sizeof(buf));
    if(ptr == NULL)
    {
        close(sockfd);
        return -1;
    }

    close(sockfd);
    strncpy(result,buf,sizeof(buf));

    return 1;
}

int get_local_ipbydomain(const char *domain,char *result)
{
    struct hostent *pHost = NULL;
    int j = 0;
    char *pszTemp = NULL;
    unsigned int uIP = 0;

    pHost=gethostbyname(domain);
    if(pHost == NULL)
    {
        return -1;
    }

    pszTemp = (char*)&uIP;
    for(j=0; j<1&&NULL!=*(pHost->h_addr_list); pHost->h_addr_list++,j++)
    {
        memcpy(pszTemp,*(pHost->h_addr_list),pHost->h_length);
        break;
    }

    /**/
    char buf[64] = "";
    const char *ptr = inet_ntop(AF_INET,&uIP,buf,sizeof(buf));
    if(ptr == NULL)
    {
        return -1;
    }

    strncpy(result,buf,sizeof(buf));

    return 1;
}

int get_remote_ipaddr(const char *devname, char *result)
{
    if(!devname || !result)
        return -1;

    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd < 0)
        return -1;

    struct ifreq ifr;
    memset(&ifr,0,sizeof(ifr));
    strcpy(ifr.ifr_name,devname);

    if(ioctl(sockfd,SIOCGIFDSTADDR,&ifr) < 0)
    {
        close(sockfd);
        return -1;
    }


    struct sockaddr_in *pAddr;
    pAddr = (struct sockaddr_in *)&(ifr.ifr_addr);
    char addr[64] = "";

    const char *ptr = inet_ntop(AF_INET,&(pAddr->sin_addr),addr,sizeof(addr));
    if(ptr == NULL)
    {
        close(sockfd);
        return -1;
    }

    close(sockfd);
    strncpy(result,addr,sizeof(addr));

    return 1;
}

int check_local_netname(const char *devname)
{
    if(!devname)
        return -1;

    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd < 0)
        return -1;

    struct ifconf ifc;
    struct ifreq ifr[10];
    ifc.ifc_len = sizeof(struct ifreq) * 10;
    ifc.ifc_buf = (char *)ifr;

    int ret = ioctl(sockfd,SIOCGIFCONF,(char *)&ifc);
    if(ret < 0)
    {
        close(sockfd);
        return -1;
    }


    int if_count = ifc.ifc_len / (sizeof(struct ifreq));
    int i;
    for(i = 0; i < if_count; i++)
    {
        if(strcmp(ifr[i].ifr_name,devname) == 0)
        {
            close(sockfd);
            return 1;
        }

    }

    close(sockfd);
    return -1;
}

int set_interface_up(const char *devname)
{
    if(!devname)
        return -1;

    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd < 0)
        return -1;

    struct ifreq ifr;
    strcpy(ifr.ifr_name,devname);
    short flag;
    flag = IFF_UP;

    if(ioctl(sockfd,SIOCGIFFLAGS,&ifr) < 0)
    {
        close(sockfd);
        return -1;
    }


    ifr.ifr_ifru.ifru_flags |= flag;
    if(ioctl(sockfd,SIOCSIFFLAGS,&ifr) < 0)
    {
        close(sockfd);
        return -1;
    }


    close(sockfd);
    return 1;
}

int set_interface_down(const char *devname)
{
    if(!devname)
        return -1;

    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd < 0)
        return -1;

    struct ifreq ifr;
    strcpy(ifr.ifr_name,devname);
    short flag;
    flag = ~IFF_UP;

    if(ioctl(sockfd,SIOCGIFFLAGS,&ifr) < 0)
    {
        close(sockfd);
        return -1;
    }

    ifr.ifr_ifru.ifru_flags &= flag;
    if(ioctl(sockfd,SIOCSIFFLAGS,&ifr) < 0)
    {
        close(sockfd);
        return -1;
    }

    close(sockfd);
    return 1;
}
