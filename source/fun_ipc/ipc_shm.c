#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "ipc.h"

#define SHM_KEY 987654321
#define SHM_SIZE    1024


struct shm_ctx {
    int shmid;
    void *parent;
};


static void *shm_init(struct ipc *ipc, uint16_t port, enum ipc_role role)
{
    struct shm_ctx *ctx = calloc(1,sizeof(struct shm_ctx));
    if(!ctx)
        return NULL;

    int shmid = shmget(SHM_KEY,SHM_SIZE,IPC_CREAT | IPC_EXCL | 0666);
    if(shmid < 0)
    {
        goto error;
    }

    ctx->parent = ipc;
    ipc->ctx = ctx;
    ctx->shmid = shmid;

    return ctx;
error:
    if(ctx)
        free(ctx);
    return NULL;
}


static void shm_deinit(struct ipc *ipc)
{
    if(ipc && ipc->ctx)
    {
        struct shm_ctx *ctx = (struct shm_ctx *)ipc->ctx;
        shmctl(ctx->shmid,IPC_RMID,0);
    }
}


static int shm_write(struct ipc *ipc, const void *buf, size_t len)
{
    struct shm_ctx *ctx = (struct shm_ctx *)ipc->ctx;
    void *shm = shmat(ctx->shmid,0,0);
    if(shm == (void *)-1)
    {
        return -1;
    }

    memcpy(shm,buf,(len > SHM_SIZE ? SHM_SIZE : len));

    shmdt(shm);

    return 0;
}

static int shm_read(struct ipc *ipc, void *buf, size_t len)
{
    struct shm_ctx *ctx = (struct shm_ctx *)ipc->ctx;
    void *shm = shmat(ctx->shmid,0,0);
    if(shm == (void *)-1)
    {
        return -1;
    }

    memcpy(buf,shm,(len > SHM_SIZE ? SHM_SIZE : len));

    shmdt(shm);

    return 0;
}


struct ipc_ops shm_ops = {
    .init             = shm_init,
    .deinit           = shm_deinit,
    .accept           = NULL,
    .connect          = NULL,
    .register_recv_cb = NULL,
    .send             = shm_write,
    .recv             = shm_read,
};
