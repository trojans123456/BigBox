#include <stdio.h>
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#ifdef __linux__
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/un.h>
#include <net/if.h>
#endif
#include <unistd.h>
#include <errno.h>
#include "stringbuf.h"
#include "net.h"

/* event 变化回调结构体*/
typedef struct
{
    sock_writing_cb_ptr func;
    void *ctx;
}writing_func_t;

/* sock 对象*/
typedef struct
{
    int fd;
    strbuf_t tx_buf; /*tcp发送缓冲*/
    hMutex mut;
    int ready;/*当前的ready事件 */
    int connected; /*是否已经连接*/
    int listening; /*是否已经连接*/
    int close_async; /*异步关闭*/
    int detach;/*分离*/
    void *context; /*该sock对象的私有数据*/
    int ios_id; /* for 多路复用*/
    enum sock_type type;
    writing_func_t wf;
    /* udp */
    IPAddress rmt_addr;
    SockHook hook;
}sock_t;

/*用于更改epoll的设置 ctl_mod 读写*/
static void set_writing(sock_t *s)
{
    if(s->wf.func)
    {
        s->wf.func((hSock)s,s->wf.ctx);
    }
}

static hSock init_sock(int fd,int bufsize,int connected)
{
    sock_t *s = NULL;
    s = (sock_t *)calloc(1,sizeof(sock_t));
    if(!s)
    {
        close(fd);
        return NULL;
    }

    /* non block*/
    int on = 1;
    ioctl(fd,FIONBIO,&on);

    s->fd = fd;
    s->connected = connected;
    s->mut = lapi_mutex_create();

    strbuf_init(&s->tx_buf,bufsize);

    return (hSock)s;
}

hSock sock_tcp(const IPAddress *local_addr, int bufsize)
{
    int fd = -1;
    sock_t *s = NULL;

    fd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(fd < 0)
    {
        return NULL;
    }

    /* set option */
    int on = 1;
    setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char*)&on, sizeof(on));
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));

    if(local_addr)
    {
        int ret = 0;
        struct sockaddr_in tSvrINAddr;
        memset(&tSvrINAddr, 0, sizeof(tSvrINAddr));
        tSvrINAddr.sin_family = AF_INET;
        tSvrINAddr.sin_addr.s_addr = inet_addr(local_addr->ip);
        tSvrINAddr.sin_port = htons(local_addr->port);

        ret = bind(fd, (struct sockaddr *)&tSvrINAddr, sizeof(tSvrINAddr));
        if (ret < 0)
        {
            printf("bind[%s:%d] error, errno = %d\n", local_addr->ip, local_addr->port, errno);
            close(fd);
            return NULL;
        }
    }

    s = (sock_t*)init_sock(fd,bufsize,0);
    if(s)
        s->type = SOCK_TYPE_TCP;

    return (hSock)s;
}

hSock sock_udp(const IPAddress *local_addr)
{
    int fd = -1;
    sock_t *s = NULL;

    fd = (int)socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (fd < 0)
    {
        printf("socket fail, errno = %d\n", errno);
        return NULL;
    }
    /**
     *udp
     * 1. sendto 指定对端地址发送数据
     * 2. connect 为套接字指定目的地址和端口 sendto函数的目的地址和端口参数需为NULL
     * 3. bind (套接字与本地ip和port绑定)客户端bind;如果服务端启动后先给客户端发送消息,则需要客户端bind
     *          (不bind则由系统自动分配ip和port)
     *
     * 4. 服务端connect  服务端只接收connect绑定的客户端的数据请求
     *
    */
    if (local_addr)
    {
        int ret = 0;

        struct sockaddr_in tSvrINAddr;
        memset(&tSvrINAddr, 0, sizeof(tSvrINAddr));
        tSvrINAddr.sin_family = AF_INET;
        tSvrINAddr.sin_addr.s_addr = inet_addr(local_addr->ip);
        tSvrINAddr.sin_port = htons(local_addr->port);

        ret = bind(fd, (struct sockaddr *)&tSvrINAddr, sizeof(tSvrINAddr));
        if (ret < 0)
        {
            printf("bind[%s:%d] error, errno = %d\n", local_addr->ip, local_addr->port, errno);
            close(fd);
            return NULL;
        }
    }

    s = (sock_t *)init_sock(fd, 0, 1);
    if(s)
        s->type = SOCK_TYPE_UDP;

    return (hSock)s;
}

hSock sock_unix(const char *path)
{
    sock_t *sock = NULL;
    struct sockaddr_un sun;
    sun.sun_family = AF_UNIX;
    if(!path)
        return NULL;
    if(strlen(path) >= sizeof(sun.sun_path))
        return NULL;

    strcpy(sun.sun_path,path);

    int fd = socket(AF_UNIX,SOCK_STREAM,0);
    if(fd < 0)
        return NULL;
    sock = (sock_t*)init_sock(fd,0,0);
    if(sock)
        sock->type = SOCK_TYPE_UNIX;

    return (hSock)sock;
}

hSock sock_can(const char *name)
{
    sock_t *s = NULL;
    int fd;
    struct sockaddr_can addr;
    struct ifreq ifr;

    if(!name)
        return NULL;

    fd = socket(AF_CAN,SOCK_RAW,CAN_RAW);
    if(fd < 0)
        return NULL;

    strcpy(ifr.ifr_name,name);
    if(ioctl(fd,SIOCGIFINDEX,&ifr) < 0)
    {
        close(fd);
        return NULL;
    }

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if(bind(fd,(struct sockaddr *)&addr,sizeof(addr)) < 0)
    {
        close(fd);
        return NULL;
    }

    s = (sock_t*)init_sock(fd,0,0);
    if(s)
        s->type = SOCK_TYPE_CAN;
    return (hSock)s;
}

void sock_close(hSock h)
{
    sock_t *s = (sock_t *)h;
    if(s)
    {
        if(s->fd != -1)
        {
            if(!s->detach)
                close(s->fd);
        }
        lapi_mutex_unlock(s->mut);
        strbuf_release(&s->tx_buf);

        free(s);
    }
}

void sock_close_async(hSock h)
{
    sock_t *s = (sock_t *)h;
    if(s)
    {
        s->close_async = 1;
        /*当可写时执行异步关闭*/
        set_writing(s);
    }
}

int sock_listen(hSock h, int max)
{
    int ret;
    sock_t *s = (sock_t *)h;

    if (!s)
    {
        return -1;
    }

    s->listening = 1;

    ret = (listen(s->fd, max));
    if (ret < 0)
    {
        printf("listen error, errno = %d\n", errno);
        return -1;
    }

    return 0;
}

hSock sock_accept(hSock h, int bufsize)
{
    struct sockaddr_in tAddr;
    unsigned int nLen;
    int new_fd;
    sock_t *s = (sock_t *)h;

    if (!s)
    {
        return NULL;
    }

    memset(&tAddr, 0, sizeof(tAddr));
    nLen = (unsigned int)sizeof(tAddr);

    new_fd = (int)accept(s->fd, (struct sockaddr *)&tAddr, &nLen);
    if (new_fd < 0)
    {
        printf("accept error, errno = %d\n", errno);
        return NULL;
    }

    return init_sock(new_fd, bufsize, 1);
}

int sock_connect(hSock h, const IPAddress *remote_addr)
{
    int ret;
    sock_t *s = (sock_t *)h;
    struct sockaddr_in tSvrINAddr;

    if (!s || !remote_addr)
    {
        return -1;
    }

    memcpy(&s->rmt_addr, remote_addr, sizeof(s->rmt_addr));

    // already connected
    if (s->connected)
    {
        return 0;
    }

    memset(&tSvrINAddr, 0, sizeof(tSvrINAddr));
    tSvrINAddr.sin_family = AF_INET;
    tSvrINAddr.sin_addr.s_addr = inet_addr(remote_addr->ip);
    tSvrINAddr.sin_port = htons(remote_addr->port);

    ret = connect(s->fd, (struct sockaddr *)&tSvrINAddr, sizeof(tSvrINAddr));
    if (ret == -1)
    {
        if (errno == EWOULDBLOCK || errno == EINPROGRESS)
        {
            ret = 0;
        }
        else
        {
            printf("connect error, errno = %d\n", errno);
        }
    }

    if (ret == 0)
    {
        // 异步连接需要设置写集
        set_writing(s);
    }

    return ret;
}

int sock_connect_sync(hSock h, const IPAddress *remote_addr, int timeout)
{
    int ret;
    sock_t *s = (sock_t *)h;

    if (!s || !remote_addr)
    {
        return -1;
    }

    memcpy(&s->rmt_addr, remote_addr, sizeof(s->rmt_addr));

    // already connected
    if (s->connected)
    {
        return 0;
    }

    ret = sock_connect(h, remote_addr);
    if (ret < 0)
    {
        return -1;
    }

    // already connected
    if (ret == 1)
    {
        return 0;
    }

    ret = sock_wait(h, timeout);
    if (ret < 0)
    {
        return -1;
    }

    if (sock_get_ready(h) != SOCK_CONNECT_OK)
    {
        printf("connect[%s:%d] fail\n", remote_addr->ip, remote_addr->port);
        return -1;
    }

    return 0;
}

int sock_dns_reverse(const char *domain, char *result)
{
    if(!domain || !result)
        return -1;

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

    return 0;
}

int sock_sendto(hSock h, const IPAddress *remote_addr, const char *buf, int len)
{
    int ret;
    struct sockaddr_in tAddr;
    sock_t *s = (sock_t *)h;

    if (!s || !buf || len <= 0 || !remote_addr)
    {
        return -1;
    }

    if (s->hook.context)
    {
        ret = s->hook.sendto(s->hook.context, h, buf, len, remote_addr);
        if (ret != 0)
        {
            return ret;
        }
    }

    memset(&tAddr, 0, sizeof(tAddr));
    tAddr.sin_family = AF_INET;
    tAddr.sin_addr.s_addr = inet_addr(remote_addr->ip);
    tAddr.sin_port = htons(remote_addr->port);

    ret = sendto(s->fd, buf, len, 0, (struct sockaddr *)&tAddr, sizeof(tAddr));
    if (ret < 0)
    {
        printf("sendto[%s:%d] error, errno = %d\n", remote_addr->ip, remote_addr->port, errno);
    }

    return ret;
}

int sock_recvfrom(hSock h, IPAddress *remote_addr, char *buf, int len)
{
    int ret;
    struct sockaddr_in tAddr;
    unsigned int addrLen = (unsigned int)sizeof(tAddr);
    sock_t *s = (sock_t *)h;

    if (!s || !buf || len <= 0 || !remote_addr)
    {
        return -1;
    }

    if (s->hook.context)
    {
        ret = s->hook.recvfrom(s->hook.context, h, buf, len, remote_addr);
        if (ret != 0)
        {
            return ret;
        }
    }

    ret = recvfrom(s->fd, buf, len, 0, (struct sockaddr*)&tAddr, &addrLen);
    if (ret < 0)
    {
        if (errno == EAGAIN)
        {
            ret = 0;
        }
        else
        {
            printf("recvfrom error, errno = %d\n", errno);
        }
    }
    else
    {
        unsigned char *ip = (unsigned char *)&(tAddr.sin_addr.s_addr);
        sprintf(remote_addr->ip, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
        remote_addr->port = (int)ntohs(tAddr.sin_port);
    }

    return ret;
}

static void tcp_resend(sock_t *s)
{
    strbuf_t *fb = &s->tx_buf;
    if (fb->len > 0)
    {
        //如果没有发送完呢？
        int ret = send(s->fd, fb->buf, fb->len, 0);
        if (ret > 0)
        {
            fb->consume(fb, ret);
        }
    }
}

static int sock_send_internal(sock_t *s, const char *buf, int len, int async)
{
    int ret;
    strbuf_t *fb = &s->tx_buf;

    // 若缓冲有数据，先发一下缓冲内的
    if (fb->len > 0)
    {
        tcp_resend(s);
    }

    // 若缓冲还有数据，或者需要异步发送
    if (fb->len > 0 || async)
    {
        if (fb->append(fb, buf, len) < 0)
        {
            printf("Tcp send too large.\n");
            return -1;
        }

        // 异步发送需要设置写集
        set_writing(s);

        return len;
    }

    // 若缓冲中无数据，直接发送
    ret = send(s->fd, buf, len, 0);
    if (ret < 0)
    {
        // 这种情况是表明发不出去，也就是说发送字节数为0
        if (errno == EWOULDBLOCK)
        {
            ret = 0;
        }
        // 其他情况表示socket有错误
        else
        {

            printf("send error, fd=%d errno = %d\n",s->fd,  errno);
            return -1;
        }
    }

    // 发送字节数不到应发的长度，需要保存剩下的数据到缓冲内
    if (ret < len)
    {
        if (fb->append(fb, buf + ret, len - ret) < 0)
        {
            printf("Tcp send incomplete.\n");
            return ret;//返回实际发送长度
        }
    }

    return len;
}

int sock_send(hSock h, const char *buf, int len)
{
    int ret;
    sock_t *s = (sock_t *)h;

    if (s->type == SOCK_TYPE_UDP)
    {
        return sock_sendto(h, &s->rmt_addr,buf, len);
    }

    if (!s || !buf || len <= 0)
    {
        return -1;
    }

    // 防止和resend竞争，需要加锁
    lapi_mutex_lock(s->mut);
    ret = sock_send_internal(s, buf, len, 0);
    lapi_mutex_unlock(s->mut);

    return ret;
}

int sock_send_async(hSock h, char *buf, int len)
{
    int ret;
    sock_t *s = (sock_t *)h;

    if (s->type == SOCK_TYPE_UDP)
    {
        return sock_sendto(h, &s->rmt_addr,buf, len);
    }

    if (!s || !buf || len <= 0)
    {
        return -1;
    }

    // 防止和resend竞争，需要加锁
    lapi_mutex_lock(s->mut);
    ret = sock_send_internal(s, buf, len, 1);
    lapi_mutex_unlock(s->mut);

    return ret;
}

int sock_recv(hSock h, char *buf, int len)
{
    int recvlen = 0;
    sock_t *s = (sock_t *)h;

    if (!s || !buf || len <= 0)
    {
        return -1;
    }

    if (s->ready == SOCK_CLOSE)
    {
        return -1;
    }

    recvlen = recv(s->fd, buf, len, 0);
    if (recvlen < 0)
    {
        if (errno == EAGAIN)
        {
            recvlen = 0;
        }
        else
        {
            printf("recv error, errno = %d\n", errno);
        }
    }

    return recvlen;
}

int sock_set_sndbuf(hSock h, int size)
{
    sock_t *s = (sock_t *)h;

    if (!s)
    {
        return -1;
    }

    setsockopt(s->fd, SOL_SOCKET, SO_SNDBUF, (char *)&size, sizeof(size));

    return 0;
}

int sock_set_rcvbuf(hSock h, int size)
{
    sock_t *s = (sock_t *)h;

    if (!s)
    {
        return -1;
    }

    setsockopt(s->fd, SOL_SOCKET, SO_RCVBUF, (char *)&size, sizeof(size));

    return 0;
}

int sock_set_reused(hSock h)
{
    sock_t *s = (sock_t *)h;
    if(!s)
        return -1;

    const int one = 1;
    setsockopt(s->fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    return 0;
}

int sock_get_remoteaddr(hSock h, IPAddress *addr)
{
    struct sockaddr_in tAddr;
    unsigned int nLen;
    int ret;
    sock_t *s = (sock_t *)h;
    unsigned char *ip;

    if (!s || !addr)
    {
        return -1;
    }

    memset(&tAddr, 0, sizeof(tAddr));
    nLen = (unsigned int)sizeof(tAddr);

    ret = getpeername(s->fd, (struct sockaddr*)&tAddr, &nLen);
    if (ret != 0)
    {
        return -1;
    }

    ip = (unsigned char *)&(tAddr.sin_addr.s_addr);
    sprintf(addr->ip, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    addr->port = (int)ntohs(tAddr.sin_port);

    return 0;
}

int sock_get_localaddr(hSock h, IPAddress *addr)
{
    struct sockaddr_in tAddr;
    unsigned int nLen;
    int ret;
    sock_t *s = (sock_t *)h;
    unsigned char *ip;

    if (!s || !addr)
    {
        return -1;
    }

    memset(&tAddr, 0, sizeof(tAddr));
    nLen = (unsigned int)sizeof(tAddr);

    ret = getsockname(s->fd, (struct sockaddr*)&tAddr, &nLen);
    if (ret != 0)
    {
        return -1;
    }

    ip = (unsigned char *)&(tAddr.sin_addr.s_addr);
    sprintf(addr->ip, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    addr->port = (int)ntohs(tAddr.sin_port);

    return 0;
}

void sock_set_context(hSock h, void *ctx)
{
    sock_t *s = (sock_t*)h;
    if(s)
    {
        s->context = ctx;
    }
}

void *sock_get_context(hSock h)
{
    sock_t *s = (sock_t*)h;

    return s ? s->context : NULL;
}

int sock_get_fd(hSock h)
{
    sock_t *s = (sock_t*)h;

    return s ? s->fd : -1;
}

int sock_detach_fd(hSock h)
{
    int fd;
    sock_t *s = (sock_t *)h;
    if (!s)
    {
        return -1;
    }

    fd = s->fd;

    // detach
    s->detach = 1;
    printf("socket[%d] has been detached!\n", s->fd);

    return fd;
}

int sock_set_hook(hSock h, SockHook *hook)
{
    sock_t *s = (sock_t *)h;
    if (!s)
    {
        return -1;
    }

    s->hook = *hook;

    return 0;
}

int sock_get_tcptxbs(hSock h)
{
    sock_t *s = (sock_t *)h;
    if(!s)
        return -1;
    return s->tx_buf.capacity;
}

int sock_get_tcptxbs_used(hSock h)
{
    sock_t *s = (sock_t *)h;
    if (!s)
    {
        return -1;
    }

    return s->tx_buf.len;
}


/********* io multi **/
#ifndef FD_SETSIZE
#define FD_SETSIZE 64
#endif

#ifdef WIN32

#define MAX_SELECT_NFD	FD_SETSIZE

static int i_select(hSock *sock, int num, int timeout, hSock *evarr)
{
    fd_set rd_set;
    fd_set wr_set;
    struct timeval tv, *ptv = NULL;
    int ev, evcount = 0;
    int i;
    int maxfd = 0;

    if (timeout >= 0)
    {
        tv.tv_sec = timeout/1000;
        tv.tv_usec = (timeout % 1000) * 1000;
        ptv = &tv;
    }

    FD_ZERO(&rd_set);
    FD_ZERO(&wr_set);

    for (i = 0; i < num; i++)
    {
        int fd = sock_get_fd(sock[i]);
        //当前socket是否可读
        if (sock_is_reading(sock[i]))
        {
            FD_SET(fd, &rd_set);
            if (fd > maxfd) maxfd = fd;
        }
        //当前socket是否可写
        if (sox_is_writing(sock[i]))
        {
            FD_SET(fd, &wr_set);
            if (fd > maxfd) maxfd = fd;
        }
    }

    if (maxfd == 0)
    {
        return 0;
    }

    ev = select(maxfd + 1, &rd_set, &wr_set, NULL, ptv);
    if (ev < 0)
    {
        printf("select error, errno = %d\n", errno);
        return -1;
    }

    if (ev == 0)
    {
        return 0;
    }

    for (i = 0; i < num; i++)
    {
        int fd = sock_get_fd(sock[i]);
        int flag = 0;

        if (FD_ISSET(fd, &rd_set))
        {
            sock_check_event(sock[i], SOCK_READABLE);
            flag = 1;
        }

        if (FD_ISSET(fd, &wr_set))
        {
            sock_check_event(sock[i], SOCK_WRITEABLE);
            flag = 1;
        }

        if (flag && sock_get_ready(sock[i]) != SOCK_NONE)
        {
            if (evarr)
            {
                evarr[evcount] = sock[i];
            }

            evcount++;
        }
    }

    return evcount;//输出的是数组个数
}

#else

#include <sys/poll.h>
#define MAX_SELECT_NFD	64

static int i_select(hSock *arr, int num, int timeout, hSock *evarr)
{
    struct pollfd fds[MAX_SELECT_NFD] = {{0}};
    int i, count = 0;
    int ev, evcount = 0;

    for (i = 0; i < num; i++)
    {
        struct pollfd tmpfd;

        tmpfd.fd = sock_get_fd(arr[i]);
        tmpfd.events = 0;

        if (sock_is_reading(arr[i]))
        {
            tmpfd.events |= POLLIN;
        }
        if (sock_is_writing(arr[i]))
        {
            tmpfd.events |= POLLOUT;
        }

        if (tmpfd.events)
        {
            fds[count].fd = tmpfd.fd;
            fds[count].events = tmpfd.events;
            count++;
        }
    }

    if (count == 0)
    {
        return 0;
    }

    ev = poll(fds, count, timeout);
    if (ev < 0)
    {
        printf("poll error, errno = %d\n", errno);
        return -1;
    }

    if (ev == 0)
    {
        return 0;
    }

    for (i = 0; i < count; i++)
    {
        int flag = 0;

        if (fds[i].revents & POLLIN)
        {
            sock_check_event(arr[i], SOCK_READABLE);
            flag = 1;
        }

        if (fds[i].revents & POLLOUT)
        {
            sock_check_event(arr[i], SOCK_WRITEABLE);
            flag = 1;
        }

        if (flag && sock_get_ready(arr[i]) != SOCK_NONE)
        {
            if (evarr)
            {
                evarr[evcount] = arr[i];
            }

            evcount++;
        }
    }

    return evcount;//输出的是数组个数
}

#endif

int sock_select(hSock *h, int num, int timeout, hSock *evarr)
{
    if(!h || num <= 0)
        return -1;
    if(num > FD_SETSIZE)
    {
        return -1;
    }

    return i_select(h,num,timeout,evarr);
}

int sock_wait(hSock h, int timeout)
{
    sock_t *s = (sock_t *)h;
    if (!s)
    {
        return -1;
    }

    if (s->hook.context)
    {
        int ret = s->hook.wait(s->hook.context, h, timeout);
        if (ret != 0)
        {
            return ret;
        }
    }

    return sock_select(&h, 1, timeout, NULL);
}

int sock_get_ready(hSock h)
{
    sock_t *s = (sock_t*)h;
    if(!s)
        return -1;
    return s->ready;
}

void sock_reset_ready(hSock h)
{
    sock_t *s = (sock_t *)h;

    if (s)
    {
        s->ready = SOCK_NONE;
    }
}

/* 检查 socket recvbuf 是否有数据*/
static int socket_avail(sock_t *s)
{
    int len = 0;
    /* 返回缓冲区有多少个字节 FIONREAD */
    ioctl(s->fd,FIONREAD,&len);
    return len;
}

void sock_check_event(hSock h, int ev)
{
    sock_t *s = (sock_t *)h;

    if (ev == SOCK_READABLE)
    {
        // 监听可读，说明有套接字连接过来
        if (s->listening)
        {
            s->ready = SOCK_NEW_SOCKET;
        }
        // 可读，但没有数据，说明对端关闭
        else if (socket_avail(s) <= 0)
        {
            s->ready = SOCK_CLOSE;
        }
        // 可读，有数据
        else
        {
            s->ready = SOCK_DATA;
        }
    }
    else if (ev == SOCK_WRITEABLE)
    {
        // 可写，检查异步关闭标识，此时回调CLOSE
        if (s->close_async)
        {
            s->ready = SOCK_CLOSE;
        }
        // 可写，并且是连接状态，可以重发TCP缓冲
        else if (s->connected)
        {
            s->ready = SOCK_RESEND;

            // 防止和send竞争，需要加锁
            lapi_mutex_lock(s->mut);
            tcp_resend(s);
            lapi_mutex_unlock(s->mut);
        }
        // 当前正在连接时发现可写，说明连接成功
        else
        {
            s->ready = SOCK_CONNECT_OK;
            s->connected = 1;
        }
    }
    else if (ev == SOCK_EXCEPTION)
    {
        s->ready = SOCK_CLOSE;
    }
}

int sock_is_reading(hSock h)
{
    sock_t *s = (sock_t *)h;

    if (!s)
    {
        return 0;
    }

    // 如果已连接上，或者是监听，则设置读集
    if (s->connected || s->listening)
    {
        return 1;
    }

    return 0;
}

int sock_is_writing(hSock h)
{
    sock_t *s = (sock_t *)h;

    if (!s)
    {
        return 0;
    }

    // TCP缓冲有数据，或者正在connect，或者设置了异步关闭标识，则需要设置写集
    if (s->tx_buf.len > 0 || !s->connected || s->close_async)
    {
        return 1;
    }

    return 0;
}

int sock_set_writing_func(hSock h, sock_writing_cb_ptr func, void *args)
{
    sock_t *s = (sock_t *)h;

    if (!s)
    {
        return -1;
    }

    s->wf.func = func;
    s->wf.ctx = args;

    return 0;
}

void sock_set_ios_id(hSock h, int ios_id)
{
    sock_t *s = (sock_t *)h;
    if (s)
    {
        s->ios_id = ios_id;
    }
}

int sock_get_ios_id(hSock h)
{
    sock_t *s = (sock_t *)h;
    return s? s->ios_id : 0;
}


static unsigned char isBigEndian()
{
    int  i = 1;   			/* i = 0x00000001*/
    char  c = *(char  *)&i; 	/* 注意不能写成 char c = (char)i; */
    return  (int )c != i;
}

unsigned long long sock_htonll(unsigned long long  val)
{
    if (isBigEndian() == 0 )
    {
        return (((unsigned long long )htonl((int)((val << 32) >> 32))) << 32) | (unsigned long long )htonl((int)(val >> 32));
    }

    return val;
}

unsigned long long sock_ntohll(unsigned long long  val)
{
    if (isBigEndian() == 0)
    {
        return (((unsigned long long  )htonl((int)((val << 32) >> 32))) << 32) | (unsigned int)htonl((int)(val >> 32));
    }

    return val;
}

/********************** IO 多路复用*************************/
typedef struct
{
    int epfd;
    int size;
    int num;
    struct epoll_event *evarr;
}EPBOX;

static void i_destroy(IOS *ios)
{
    if (ios)
    {
        EPBOX *epb = (EPBOX *)ios->pri;
        if (epb)
        {
            close(epb->epfd);

            if (epb->evarr)
            {
                free(epb->evarr);
            }

            free(epb);
        }

        if (ios->evarr)
        {
            free(ios->evarr);
        }

        lapi_mutex_destroy(ios->mtx);
    }
}

static int _modify_writing_set(IOS *ios, hSock h, int wr)
{
    EPBOX *epb = (EPBOX *)ios->pri;
    struct epoll_event ev = {0};
    int fd = sock_get_fd(h);
    int ret = 0;

    ev.data.ptr = (void *)h;
    ev.events = EPOLLIN;//固定设置读集

    if (wr)
    {
        ev.events |= EPOLLOUT;
    }
    else
    {
        ev.events &= ~EPOLLOUT;
    }

    lapi_mutex_lock(ios->mtx);
    ret = epoll_ctl(epb->epfd, EPOLL_CTL_MOD, fd, &ev);
    lapi_mutex_unlock(ios->mtx);

// 	printf("_modify_writing_set %d\n", wr);

    return ret;
}

static void func_set_writing(hSock h, void *ctx)
{
    IOS *ios = (IOS *)ctx;
    if (ios)
    {
        _modify_writing_set(ios, h, 1);
    }
}

static int i_add(IOS *ios, hSock h)
{
    EPBOX *epb = (EPBOX *)ios->pri;
    struct epoll_event ev = {0};
    int fd = sock_get_fd(h);
    int ret = 0;

    if (!ios)
    {
        return -1;
    }

    if (epb->num >= epb->size)
    {
        return -1;
    }

    ev.data.ptr = (void *)h;
    ev.events = EPOLLIN;//固定设置读集

    lapi_mutex_lock(ios->mtx);

    ret = epoll_ctl(epb->epfd, EPOLL_CTL_ADD, fd, &ev);
    if (ret == 0)
    {
        epb->num++;
    }

    lapi_mutex_unlock(ios->mtx);

    // 另外，设置监听写集函数
    sock_set_writing_func(h, func_set_writing, (void *)ios);

    return ret;
}

static int i_del(IOS *ios, hSock h)
{
    EPBOX *epb = (EPBOX *)ios->pri;
    int fd = sock_get_fd(h);
    int ret = 0;

    if (!ios)
    {
        return -1;
    }

    lapi_mutex_lock(ios->mtx);

    ret = epoll_ctl(epb->epfd, EPOLL_CTL_DEL, fd, NULL);
    if (ret == 0)
    {
        epb->num--;
    }

    lapi_mutex_unlock(ios->mtx);

    return ret;
}

static int i_ioswitch(IOS *ios,int timeout)
{
    int ev = 0;
    EPBOX *epb = (EPBOX *)ios->pri;
    int i;

    if (!ios)
    {
        return -1;
    }

    ev = epoll_wait(epb->epfd, epb->evarr, epb->size, timeout);

    if (ev < 0)
    {
        //printf("epoll_wait err[%d]\n", errno);
        return -1;
    }

    for (i = 0; i < ev; i++)
    {
        hSock sox = (hSock)epb->evarr[i].data.ptr;

        if (epb->evarr[i].events & EPOLLERR)
        {
            sock_check_event(sox, SOCK_EXCEPTION);
        }

        if (epb->evarr[i].events & EPOLLHUP)
        {
            if (sock_is_reading(sox))
            {
                sock_check_event(sox, SOCK_EXCEPTION);
            }
        }

        if (epb->evarr[i].events & EPOLLIN)
        {
            sock_check_event(sox, SOCK_READABLE);
        }

        if (epb->evarr[i].events & EPOLLOUT)
        {
            sock_check_event(sox, SOCK_WRITEABLE);

            // 如果sox不再需要监听写集，则去掉写集
            if (!sock_is_writing(sox))
            {
                _modify_writing_set(ios, sox, 0);
            }
        }

        ios->evarr[i] = sox;
    }

    return ev;
}

int ios_epoll_init(IOS *ios, int maxfds)
{
    EPBOX *epb;

    if (!ios)
    {
        return -1;
    }

    ios->mtx = lapi_mutex_create();

    ios->destroy = i_destroy;
    ios->add = i_add;
    ios->del = i_del;
    ios->ioswitch = i_ioswitch;

    ios->evarr = (hSock *)calloc(maxfds, sizeof(hSock));
    if (!ios->evarr)
    {
        printf("malloc evarr fail\n");
        return -1;
    }

    epb = (EPBOX *)malloc(sizeof(EPBOX));
    if (!epb)
    {
        printf("malloc epb fail\n");
        return -1;
    }
    ios->pri = (void *)epb;

    epb->epfd = epoll_create(maxfds);
    if (epb->epfd < 0)
    {
        printf("create epoll failed\n");
        return -1;
    }

    epb->size = maxfds;
    epb->num = 0;
    epb->evarr = (struct epoll_event *)calloc(maxfds, sizeof(struct epoll_event));
    if (!epb->evarr)
    {
        printf("malloc epb->evarr fail\n");
        return -1;
    }

    return 0;
}
