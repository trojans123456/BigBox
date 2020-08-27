#ifndef SOCKET_NET_H
#define SOCKET_NET_H

//http://blog.chinaunix.net/uid-26193917-id-4990763.html

#include <sys/types.h>

#define ANET_OK 0
#define ANET_ERR -1
#define ANET_ERR_LEN 256

/* Flags used with certain functions. */
#define ANET_NONE 0
#define ANET_IP_ONLY (1<<0)

//#if defined(__sun) || defined(_AIX)
//#define AF_LOCAL AF_UNIX
//#endif

//#ifdef _AIX
//#undef ip_len
//#endif

/*
*@brief 连接服务器。阻塞
*@params
*   err[out] 返回错误信息
*   addr[in] 域名或地址或名称 getaddrinfo
*   port[in] 端口号
*/
int anetTcpConnect(char *err, char *addr, int port);
int anetTcpNonBlockConnect(char *err, char *addr, int port);
/*
*@brief 连接服务器，非阻塞绑定
*@params
*   err[out] 返回错误信息
*   addr[in] 域名或地址
*   port[in] 端口号
*   source_addr [in] 要绑定的地址。注意和addr不一定相同
*/
int anetTcpNonBlockBindConnect(char *err, char *addr, int port, char *source_addr);
/*尽最大努力绑定。相当于递归绑定*/
int anetTcpNonBlockBestEffortBindConnect(char *err, char *addr, int port, char *source_addr);
int anetUnixConnect(char *err, char *path);
int anetUnixNonBlockConnect(char *err, char *path);
/*
*@brief 读数据
*/
int anetRead(int fd, char *buf, int count);
/*获取主机名的ip地址*/
int anetResolve(char *err, char *host, char *ipbuf, size_t ipbuf_len);
/*获取数字串地址的ip地址*/
int anetResolveIP(char *err, char *host, char *ipbuf, size_t ipbuf_len);
/*启动tcp server*/
int anetTcpServer(char *err, int port, char *bindaddr, int backlog);
int anetTcp6Server(char *err, int port, char *bindaddr, int backlog);
int anetUnixServer(char *err, char *path, mode_t perm, int backlog);
/*accept*/
int anetTcpAccept(char *err, int serversock, char *ip, size_t ip_len, int *port);
int anetUnixAccept(char *err, int serversock);
int anetWrite(int fd, char *buf, int count);
/*设置非阻塞*/
int anetNonBlock(char *err, int fd);
/*设置为阻塞*/
int anetBlock(char *err, int fd);
int anetEnableTcpNoDelay(char *err, int fd);
int anetDisableTcpNoDelay(char *err, int fd);
int anetTcpKeepAlive(char *err, int fd);
int anetSendTimeout(char *err, int fd, long long ms);
int anetPeerToString(int fd, char *ip, size_t ip_len, int *port);
int anetKeepAlive(char *err, int fd, int interval);
/*获取fd的ip和端口号*/
int anetSockName(int fd, char *ip, size_t ip_len, int *port);
int anetFormatAddr(char *fmt, size_t fmt_len, char *ip, int port);
int anetFormatPeer(int fd, char *fmt, size_t fmt_len);
int anetFormatSock(int fd, char *fmt, size_t fmt_len);

#endif
