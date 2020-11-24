#ifndef __NET_H
#define __NET_H

#include "task.h"
#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

declear_handler(hSock);

/* ip addr */
typedef struct
{
    int port;
    char ip[64];
}IPAddress;

enum sock_type
{
    SOCK_TYPE_TCP,
    SOCK_TYPE_UDP,
    SOCK_TYPE_UNIX,
    SOCK_TYPE_CAN
};


/*创建句柄*/
hSock sock_tcp(const IPAddress *local_addr,int bufsize);
hSock sock_udp(const IPAddress *local_addr);
hSock sock_unix(const char *path);
hSock sock_can(const char *name);
void sock_close(hSock h);
void sock_close_async(hSock h); /*异步关闭*/

/* 监听*/
int sock_listen(hSock h,int max);
hSock sock_accept(hSock h,int bufsize);
int sock_connect(hSock h,const IPAddress *remote_addr);
int sock_connect_sync(hSock h, const IPAddress *remote_addr, int timeout);
/**域名解析*/
int sock_dns_reverse(const char *domain, char *result);

/* 传输*/
int sock_sendto(hSock h,const IPAddress *remote_addr,const char *buf,int len);
int sock_recvfrom(hSock h,IPAddress *remote_addr,char *buf,int len);
int sock_send(hSock h,const char *buf,int len);
int sock_send_async(hSock h,char *buf,int len); /*异步传输*/
int sock_recv(hSock h,char *buf,int len);

/*设置接收缓存 kernel层*/
int sock_set_sndbuf(hSock h,int size);
int sock_set_rcvbuf(hSock h,int size);
/*连接断开后ip地址马上复用*/
int sock_set_reused(hSock h);

int sock_get_remoteaddr(hSock h,IPAddress *addr);
int sock_get_localaddr(hSock h,IPAddress *addr);

void sock_set_context(hSock h,void *ctx);
void *sock_get_context(hSock h);

int sock_get_fd(hSock h);
int sock_detach_fd(hSock h); /*取出fd之后 hsock不在有fd*/

/*获取tcp可靠发送缓冲区大小 tx buf size 应用层*/
int sock_get_tcptxbs(hSock h);
/* 获取TCP可靠发送缓冲已用大小 */
int sock_get_tcptxbs_used(hSock h);

void sock_set_ios_id(hSock h,int ios_id);
int sock_get_ios_id(hSock h);

unsigned long long sock_htonll(unsigned long long val);
unsigned long long sock_ntohll(unsigned long long val);

/*sock hook */
typedef struct
{
    /* user data */
    void *context;
    /* 清理hook */
    void (*release)(void *context);
    // sox_wait前调用。若返回不等于0，则不再sox_wait。
    int (*wait)(hSock h, void *context,int timeout);
    // UDP发送前调用。若返回不等于0，则不再发送，且返回值作为实际sendto的返回值。
    int (*sendto)(hSock h,void *context,const char *buf, int len, const IPAddress *addr);
    // UDP接收前调用。若返回不等于0，则不再接收，且返回值作为实际recvfrom的返回值。
    int (*recvfrom)(hSock h,void *context,char *buf, int len, IPAddress *addr);
}SockHook;

int sock_set_hook(hSock h,SockHook *hook);

/*普通的select 返回产生事件的sock数目 evarr中保存*/
int sock_select(hSock *h,int num,int timeout,hSock *evarr);
/* 等待一个 sock发生事件 */
int sock_wait(hSock h,int timeout);

/* ready 事件 */
enum
{
    SOCK_NONE = 0,
    SOCK_DATA, /*有数据过来*/
    SOCK_NEW_SOCKET, /*有新连接过来*/
    SOCK_CONNECT_OK, /*连接成功*/
    SOCK_CLOSE, /*连接关闭*/
    SOCK_RESEND /*有重发数据*/
};

/* 获取 ready事件 */
int sock_get_ready(hSock h);
/*清空ready状态*/
void sock_reset_ready(hSock h);

/* 判断sock是否需要设置读集合*/
int sock_is_reading(hSock h);
/* 判断sock是否设置写集合*/
int sock_is_writing(hSock h);

typedef void (*sock_writing_cb_ptr)(hSock h,void *args);
int sock_set_writing_func(hSock h,sock_writing_cb_ptr func,void *args);

/*根据底层事件 判断sock处于哪个read事件*/
enum
{
    SOCK_READABLE,
    SOCK_WRITEABLE,
    SOCK_EXCEPTION
};
/*验证socket的事件*/
void sock_check_event(hSock h,int ev);


/***************** 多路复用***********************/
typedef struct _IOS
{
    void (*destroy)(struct _IOS *ios);
    int (*add)(struct _IOS *ios,hSock h);
    int (*del)(struct _IOS *ios,hSock h);
    int (*ioswitch)(struct _IOS *ios,int timeout);
    hSock *evarr;/*产生事件的sock数组*/
    void *pri;
    hMutex mtx;
}IOS;

int ios_epoll_init(IOS *ios,int maxfds);


#ifdef __cplusplus
}
#endif

#endif
