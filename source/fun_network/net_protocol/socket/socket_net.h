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
*@brief ���ӷ�����������
*@params
*   err[out] ���ش�����Ϣ
*   addr[in] �������ַ������ getaddrinfo
*   port[in] �˿ں�
*/
int anetTcpConnect(char *err, char *addr, int port);
int anetTcpNonBlockConnect(char *err, char *addr, int port);
/*
*@brief ���ӷ���������������
*@params
*   err[out] ���ش�����Ϣ
*   addr[in] �������ַ
*   port[in] �˿ں�
*   source_addr [in] Ҫ�󶨵ĵ�ַ��ע���addr��һ����ͬ
*/
int anetTcpNonBlockBindConnect(char *err, char *addr, int port, char *source_addr);
/*�����Ŭ���󶨡��൱�ڵݹ��*/
int anetTcpNonBlockBestEffortBindConnect(char *err, char *addr, int port, char *source_addr);
int anetUnixConnect(char *err, char *path);
int anetUnixNonBlockConnect(char *err, char *path);
/*
*@brief ������
*/
int anetRead(int fd, char *buf, int count);
/*��ȡ��������ip��ַ*/
int anetResolve(char *err, char *host, char *ipbuf, size_t ipbuf_len);
/*��ȡ���ִ���ַ��ip��ַ*/
int anetResolveIP(char *err, char *host, char *ipbuf, size_t ipbuf_len);
/*����tcp server*/
int anetTcpServer(char *err, int port, char *bindaddr, int backlog);
int anetTcp6Server(char *err, int port, char *bindaddr, int backlog);
int anetUnixServer(char *err, char *path, mode_t perm, int backlog);
/*accept*/
int anetTcpAccept(char *err, int serversock, char *ip, size_t ip_len, int *port);
int anetUnixAccept(char *err, int serversock);
int anetWrite(int fd, char *buf, int count);
/*���÷�����*/
int anetNonBlock(char *err, int fd);
/*����Ϊ����*/
int anetBlock(char *err, int fd);
int anetEnableTcpNoDelay(char *err, int fd);
int anetDisableTcpNoDelay(char *err, int fd);
int anetTcpKeepAlive(char *err, int fd);
int anetSendTimeout(char *err, int fd, long long ms);
int anetPeerToString(int fd, char *ip, size_t ip_len, int *port);
int anetKeepAlive(char *err, int fd, int interval);
/*��ȡfd��ip�Ͷ˿ں�*/
int anetSockName(int fd, char *ip, size_t ip_len, int *port);
int anetFormatAddr(char *fmt, size_t fmt_len, char *ip, int port);
int anetFormatPeer(int fd, char *fmt, size_t fmt_len);
int anetFormatSock(int fd, char *fmt, size_t fmt_len);

#endif
