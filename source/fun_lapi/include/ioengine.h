#ifndef __IOENGINE_H
#define __IOENGINE_H

#include "common.h"
#include "net.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 线程方式监听
 */


declear_handler(hIOEngine);

typedef void (*ioengine_event_cb_ptr)(hIOEngine ioe,hSock h,void *ctx);
typedef struct
{
    ioengine_event_cb_ptr cb;
    void *ctx;
    int listen_max;/*监听的最大数*/
}ioe_params;

hIOEngine ioengine_startup(ioe_params *param);

void ioengine_cleanup(hIOEngine h);

/*添加句柄*/
int ioengine_add(hIOEngine h,hSock sock);
int ioengine_del(hIOEngine h,hSock sock);

#ifdef __cplusplus
}
#endif

#endif
