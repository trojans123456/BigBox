#ifndef __NET_SOCKET_H
#define __NET_SOCKET_H

#include <stddef.h>
#include <stdint.h>

#ifdef __linux__
#include <linux/can.h>
#include <linux/can/raw.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* socket type */
#define SOCKET_TYPE_TCP      1
#define SOCKET_TYPE_UDP      2
#define SOCKET_TYPE_UNIX     3

/* ip type */
#define IP_TYPE_V4      1
#define IP_TYPE_V6      2
#define IP_TYPE_BOTH    3

/* for shutdown */
#define SHUTDOWN_TYPE_READ  1
#define SHUTDOWN_TYPE_WRITE 2

/* for nettype */
#define LOCAL_NET_TYPE_NONE     0
#define LOCAL_NET_TYPE_ETH      1
#define LOCAL_NET_TYPE_PPP      2
#define LOCAL_NET_TYPE_LOOP     3


/**
 * @brief create_tcp_socket 创建一个tcp并连接
 * @param hostname 连接的地址
 * @param service 连接的端口或某个服务
 * @param proto
 * @return success-返回一个有效的描述符 失败 -1
 */
int create_tcp_socket(const char *hostname,const char *service,unsigned char proto);

/**
 * @brief create_udp_socket 创建一个udp
 * @param proto ip协议
 * @return
 */
int create_udp_socket(unsigned char proto);

/**
 * @brief create_unix_socket  目前是stream形式
 * @param path
 * @return
 */
int create_unix_socket(const char *path);

int create_unix_server(const char *path, int listen_max);

/**
 * @brief udp_connect_to
 * @param sockfd
 * @param host 远程主机
 * @param service 远程服务
 * @return
 */
int udp_connect_to(int sockfd,const char *host, const char *service);

/**
 * @brief tcp_sendto
 * @param sockfd
 * @param buf 要发送的数据
 * @param buf_len 数据长度
 * @param host 远程域名或主机
 * @param service 远程端口或服务
 * @return
 */
ssize_t udp_sendto(int sockfd,void *buf,size_t buf_len,
                   const char *host,const char *service);

/**
 * @brief udp_recvfrom
 * @param sockfd
 * @param buf
 * @param buf_len
 * @param src_host 用于保存主机名
 * @param src_host_len
 * @param src_service 用于保存服务名(端口)
 * @param src_service_len
 * @return
 */
ssize_t udp_recvfrom(int sockfd,void *buf,size_t buf_len,
                     char *src_host,size_t src_host_len,
                     char *src_service,size_t src_service_len);


int close_socket(int sockfd);

/**
 * @brief shutdown_socket
 * @param sockfd
 * @param flags (SHUTDOWN_TYPE_READ or SHUTDOWN_TYPE_WRITE)
 * @return
 */
int shutdown_socket(int sockfd,int flags);

/**
 * @brief set_nonblock
 * @param sockfd
 * @return
 */
int set_nonblock(int sockfd);

int set_block(int sockfd);

/**
 * @brief 获取主机所支持的ip类型
 * @param hostname
 * @return ipv4 or ipv6
 */
int get_ip_family(const char *hostname);

/**
 * @brief create_server_socket 创建一个tcu udp 服务端
 * @param bind_addr 本地地址
 * @param bind_port
 * @param sock_type  tcp or udp
 * @param ip_type   ipv4 ipv6
 * @return 返回server 描述符
 */
int create_server_socket(const char *bind_addr,const char *bind_port,
                         unsigned char sock_type,unsigned char ip_type,
                         int listen_max);

/**
 * @brief accept_socket
 * @param sockfd  服务端fd
 * @param src_host
 * @param src_host_len
 * @param src_service
 * @param src_service_len
 * @return
 */
int accept_socket(int sockfd,char *src_host,size_t src_host_len,
                  char *src_service,size_t src_service_len);

/** 设置 keepalive 时间间隔*/
int set_opt_keepalive(int sockfd,int interval);
/** 网络拥堵算法,ok = 1开启 仅仅tcp*/
int set_opt_nodelay(int sockfd,int ok);
/** 改变缺省缓冲区大小 kernel*/
int set_opt_sendbuffer(int sockfd,int buffsize);
/** 发送数据超时时间*/
int set_opt_sendtimeout(int sockfd,long long ms);
/** 设置仅用ipv6 */
int set_opt_ipv6_only(int sockfd);
/** */
int set_opt_reuseaddr(int sockfd);

int sendall(int sockfd,void *buf,size_t buf_len);
int recvall(int sockfd,void *buf,size_t buf_len);

int readall(int sockfd,void *buf,int count);
int writeall(int sockfd,void *buf,int count);

int create_can_socket(const char *devname);
/** 禁止接收过滤规则,kernel 会丢掉该fd收到的can帧*/
void disable_can_reciverfileter(int canfd);
/** 禁止回环*/
void disable_can_loopback(int canfd);

void set_can_filter(int canfd,struct can_filter *filter,int count);

/***************** local *************/
int get_local_ipaddr(const char *devname,char *result);
int get_local_macaddr(const char *devname,char *result);
int get_local_nettype(const char *devname);
/* 子网掩码 */
int get_local_mask(const char *devname,char *result);
int get_local_ipbydomain(const char *domain,char *result);
/* 判断网络节点是否存在*/
int check_local_netname(const char *devname);

int get_remote_ipaddr(const char *devname,char *result);

int set_interface_up(const char *devname);
int set_interface_down(const char *devname);

#ifdef __cplusplus
}
#endif

#endif
