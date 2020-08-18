#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "ipc.h"

#define MKFIFO_PATH "/tmp/mkfifo"
#define MKFIFO_PATH_LEN 32

struct mkfifo_posix_ctx
{
    int fd;
    void *private_data;
    char path[MKFIFO_PATH_LEN];
    ipc_recv_cb *cb;
};

static void *mkfifo_init_(struct ipc *ipc,uint16_t port,enum ipc_role role)
{
    struct mkfifo_posix_ctx *ctx = NULL;
    ctx = calloc(1,sizeof(struct mkfifo_posix_ctx));
    if(!ctx)
    {
        goto error;
    }
    snprintf(ctx->path,sizeof(ctx->path),"%s%d",MKFIFO_PATH,port);
    int ret = mkfifo(ctx->path,S_IFIFO | 0666);
    if(ret < 0)
    {
        goto error;
    }

    ctx->fd = open(ctx->path,O_RDWR);
    if(ctx->fd < 0)
    {
        goto error;
    }

    ipc->ctx = ctx;
    ctx->private_data = ipc;

    ctx->rd_fd = pipefd[0]; /* 0 read */
    ctx->wr_fd = pipefd[1]; /* 1 write */

    ctx->cb = NULL;

    return ctx;

error:
    if(ctx)
        free(ctx);

    return NULL;
}

static int mkfifo_set_recv_cb(struct ipc *ipc,ipc_recv_cb *cb)
{
    if(ipc)
    {
        struct mkfifo_posix_ctx *ctx = (struct mkfifo_posix_ctx *)(ipc->ctx);
        if(!ctx)
        {
            return -1;
        }
        ctx->cb = cb;
    }
    return -1;
}

static void mkfifo_deinit(struct ipc *ipc)
{
    if(ipc)
    {
        struct mkfifo_posix_ctx *ctx = (struct mkfifo_posix_ctx *)(ipc->ctx);
        if(ctx)
        {
            close(ctx->fd);
            free(ctx);
        }
    }
}

static int pipe_send_(struct ipc *ipc,const void *buf,size_t len)
{
    if(!ipc || !ipc->ctx)
        return -1;

    struct mkfifo_posix_ctx *ctx = (struct mkfifo_posix_ctx *)ipc->ctx;

    return write(ctx->fd,buf,len);
}

static int pipe_recv_(struct ipc *ipc,void *buf,size_t len)
{
    if(!ipc || !ipc->ctx)
        return -1;

    struct mkfifo_posix_ctx *ctx = (struct mkfifo_posix_ctx *)ipc->ctx;
    return read(ctx->fd,buf,len);
}

struct ipc_ops mkfifo_ops =
{
    .init = mkfifo_init_,
    .deinit = mkfifo_deinit_,
    .accept = NULL,
    .connect = NULL,
    .register_recv_cb = mkfifo_set_recv_cb_,
    .send = mkfifo_send_,
    .recv = mkfifo_recv_,
};
