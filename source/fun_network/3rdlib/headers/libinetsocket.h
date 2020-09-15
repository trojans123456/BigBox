#ifndef LIBSOCKET_INETSOCKET_H
#define LIBSOCKET_INETSOCKET_H


#ifndef __WIN32__
#include <sys/socket.h>
#include <sys/types.h>
#endif

/* Macro definitions */

#define LIBSOCKET_TCP 1
#define LIBSOCKET_UDP 2

#define LIBSOCKET_IPv4 3
#define LIBSOCKET_IPv6 4

#define LIBSOCKET_BOTH \
    5 /* let the resolver/library choose (TCP/UDP or IPv4/6) */

#define LIBSOCKET_READ 1
#define LIBSOCKET_WRITE 2

#define LIBSOCKET_NUMERIC 1

#ifdef __cplusplus
extern "C" {
#endif

extern int create_inet_stream_socket(const char* host, const char* service,
                                     char proto_osi3, int flags);
extern int create_inet_dgram_socket(char proto_osi3, int flags);
extern ssize_t sendto_inet_dgram_socket(int sfd, const void* buf, size_t size,
                                        const char* host, const char* service,
                                        int sendto_flags);
extern ssize_t recvfrom_inet_dgram_socket(int sfd, void* buffer, size_t size,
                                          char* src_host, size_t src_host_len,
                                          char* src_service,
                                          size_t src_service_len,
                                          int recvfrom_flags, int numeric);
extern int connect_inet_dgram_socket(int sfd, const char* host,
                                     const char* service);
extern int destroy_inet_socket(int sfd);
extern int shutdown_inet_stream_socket(int sfd, int method);
extern int create_inet_server_socket(const char* bind_addr,
                                     const char* bind_port, char proto_osi4,
                                     char proto_osi3, int flags);
extern int accept_inet_stream_socket(int sfd, char* src_host,
                                     size_t src_host_len, char* src_service,
                                     size_t src_service_len, int flags,
                                     int accept_flags);
extern int get_address_family(const char* hostname);


extern int create_multicast_socket(const char* group, const char* port,
                                   const char* local);


#ifdef __cplusplus
}
#endif

#endif
