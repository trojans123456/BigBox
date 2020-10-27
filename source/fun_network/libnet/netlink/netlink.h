#ifndef __NETLINK_H
#define __NETLINK_H

#ifdef __cplusplus
extern "C" {
#endif

int create_netlink(int type);

int sendall(int sockfd,void *buff,int buffer_len);
int recvall(int sockfd,void *buff,int buffer_len);

#ifdef __cplusplus
}
#endif

#endif
