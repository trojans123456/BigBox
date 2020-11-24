
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>

#include "task.h"
#include "net.h"
#include "ioengine.h"
#include "tcpserver.h"

//////////////////////////////////////////////////////////////////////////
#define MAX_THREAD_NUM	1024
typedef struct
{
    TSPARAMS params;
    hSock listen_sox;		//监听套接字
    hIOEngine engine;
    hThread listen_thread;
} TTcpServer;

//////////////////////////////////////////////////////////////////////////
// listen
static void *task_listen(hThread th, void *p)
{
    int ret;
    TTcpServer *srv = (TTcpServer *)p;



    while (lapi_thread_isrunning(th))
    {
        int ev = sock_wait(srv->listen_sox, 1000);
        if (ev > 0 && sock_get_ready(srv->listen_sox) == SOCK_NEW_SOCKET)
        {
            IPAddress addr;


            hSock new_sox = sock_accept(srv->listen_sox, srv->params.sox_bufsize);
            if (!new_sox)
            {
                continue;
            }

            sock_get_remoteaddr(new_sox, &addr);

            printf("new_soc = %d\n",sock_get_fd(new_sox));

            // 先回调
            if (srv->params.cb_conn)
            {
                srv->params.cb_conn(new_sox, srv->params.context);
            }

            // 再加入引擎
            ret = ioengine_add(srv->engine, new_sox);
            if (ret < 0)
            {


                // 若加入失败则当做连接关闭回调
                if (srv->params.cb_close)
                {
                    srv->params.cb_close(new_sox, srv->params.context);
                }

                sock_close(new_sox);
            }
        }
    }


    return 0;
}

//////////////////////////////////////////////////////////////////////////
// engine callback
static void ioe_callback(hIOEngine ioe, hSock sox, void *ctx)
{
    TTcpServer *srv = (TTcpServer *)ctx;

    switch (sock_get_ready(sox))
    {
    case SOCK_CLOSE:
        {
            IPAddress addr;

            // 删除已关闭的连接
            int ret = sock_get_remoteaddr(sox, &addr);
            if (ret == 0)
            {
               printf("[tcpserver] session close: fd[%d] peer[%s:%d] ", sock_get_fd(sox), addr.ip, addr.port);
            }
            else
            {
                printf("[tcpserver] session close: fd[%d]", sock_get_fd(sox));
            }

            ioengine_del(ioe, sox);

            if (srv->params.cb_close)
            {
                srv->params.cb_close(sox, srv->params.context);
            }

            // 需要关闭本端
            sock_close(sox);
        }
        break;

    case SOCK_DATA:
        {
        printf("jfdskfjsl\n");
            // 回调有数据的连接
            if (srv->params.cb_data)
            {
                srv->params.cb_data(sox, srv->params.context);
            }
        }
        break;

    default:
        break;
    }
}

//////////////////////////////////////////////////////////////////////////
// api
hTcpServer ts_startup(IPAddress *host, TSPARAMS *params)
{
    TTcpServer *srv = NULL;
    ioe_params ioep = {0};

    if (!params)
    {
        return NULL;
    }

    srv = (TTcpServer *)malloc(sizeof(TTcpServer));
    if (!srv)
    {
        return NULL;
    }
    memset(srv, 0, sizeof(TTcpServer));

    if (params->sox_bufsize < 0)
    {
        params->sox_bufsize = 0;
    }
    srv->params = *params;

    // 创建引擎
    ioep.cb = ioe_callback;
    ioep.ctx = (void *)srv;
    ioep.listen_max = 10;

    srv->engine = ioengine_startup(&ioep);
    if (!srv->engine)
    {
        printf("[tcpserver] ioengine_startup fail!");
        return NULL;
    }

    // 创建监听
    srv->listen_sox = sock_tcp(host, 0);
    if (srv->listen_sox == NULL)
    {
        return NULL;
    }

    // 监听，backlog设置为最大连接数目
    sock_listen(srv->listen_sox, params->conn_num * params->thread_num);
    srv->listen_thread = lapi_thread_create(task_listen, srv,64<<10);

    return (hTcpServer)srv;
}

void ts_cleanup(hTcpServer h)
{
    TTcpServer *srv = (TTcpServer *)h;
    if (srv)
    {
        lapi_thread_destroy(srv->listen_thread);
        sock_close(srv->listen_sox);
        ioengine_cleanup(srv->engine);
        free(srv);
    }
}
