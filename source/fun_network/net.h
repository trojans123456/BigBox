#ifndef __NET_H
#define __NET_H


#ifdef __cplusplus
extern "C" {
#endif

#define NSOCK_TCP    0
#define NSOCK_UDP    1
#define NSOCK_UNIX   2

#define NET_SOCK_SERVER         0x0100
/* for fork */
#define NET_SOCK_NOCLOEXEC		0x0200
#define NET_SOCK_NONBLOCK		0x0400
/* 阻止域名解析 必须是一个x.x.x.x的ip地址*/
#define NET_SOCK_NUMERIC		0x0800
#define NET_SOCK_IPV6ONLY		0x2000
#define NET_SOCK_IPV4ONLY		0x4000
#define NET_SOCK_UNIX           0x8000


/**
 * @brief port转换为字符串格式
 * @param port
 * @return
 */
const char *net_sock_port(int port);

/**
 * @brief net_sock
 * @param type
 * @param host  一个主机名或者地址串(x.x.x.x) 或域名
 * @param serviece 服务名可以是十进制的端口号
 * @return
 */
int net_sock(int type, const char *host, const char *service);


int net_sock_inet_timeout(int type, const char *host, const char *service,
               void *addr, int timeout);

static inline int
net_sock_inet(int type, const char *host, const char *service, void *addr)
{
    return net_sock_inet_timeout(type, host, service, addr, -1);
}

/**
 * Wait for a socket to become ready.
 *
 * This may be useful for users of USOCK_NONBLOCK to wait (with a timeout)
 * for a socket.
 *
 * @param fd file descriptor of socket
 * @param msecs timeout in microseconds
 */
int usock_wait_ready(int fd, int msecs);


#ifdef __cplusplus
}
#endif

#endif
