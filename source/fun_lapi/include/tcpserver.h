/*
 *   TCP Server
 *
 *   设计用意：用于构建高并发的TCP服务器
 *             尽量少使用锁，以避免竞争开销
 *             同一连接始终在一个线程处理，有助于上层应用的稳定性
 */
#ifndef __TCPSERVER_H
#define __TCPSERVER_H

#include "common.h"
#include "net.h"

#ifdef __cplusplus
extern "C" {
#endif


declear_handler(hTcpServer);

typedef struct
{
    int sox_bufsize;	//每个套接字设置的发送缓冲大小
    int conn_num;		//每个线程支持的最大连接数
    int thread_num;		//总共开启的线程数
    void (*cb_conn)(hSock sox, void *context);//sox连接事件的回调函数
    void (*cb_close)(hSock sox, void *context);//使用时需要注意：回调结束后sox已销毁，不能再使用
    void (*cb_data)(hSock sox, void *context);//sox数据接事件的回调函数
    void *context; //上下文
} TSPARAMS;

//开启tcp服务器
//param: [IN]host :本地服务器IP地址,如对本地ip地址不清楚,host.ip="0.0.0.0"
//return:  !NULL:成功; NULL:失败
C_API hTcpServer ts_startup(IPAddress *host, TSPARAMS *params);

//释放tcp服务器
C_API void ts_cleanup(hTcpServer h);

#ifdef __cplusplus
}
#endif

#endif
