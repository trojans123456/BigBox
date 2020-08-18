#ifndef __IPC_H
#define __IPC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ipc;

/**
 * @brief ipc方向
 */
typedef enum ipc_role
{
    IPC_SERVER = 0,
    IPC_CLIENT = 1
}ipc_role_e;

typedef int (ipc_recv_cb)(struct ipc *a_ipc,void *buf,size_t len);
struct ipc_ops {
    void *(*init)(struct ipc *ipc, uint16_t port, enum ipc_role role);
    void (*deinit)(struct ipc *ipc);
    int (*accept)(struct ipc *ipc);
    int (*connect)(struct ipc *ipc, const char *name);
    int (*register_recv_cb)(struct ipc *i, ipc_recv_cb cb);
    int (*send)(struct ipc *i, const void *buf, size_t len);
    int (*recv)(struct ipc *i, void *buf, size_t len);
    int (*unicast)();
    int (*broadcast)();
};

struct ipc
{
    void *ctx; /*指向具体的通信机制*/
    ipc_role_e role;
    int afd;/*server fd*/
    const struct ipc_ops *ops;
};

#ifdef __cplusplus
}
#endif


#endif
