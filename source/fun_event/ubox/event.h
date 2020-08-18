#ifndef __EVENT_H
#define __EVENT_H

#include <stdint.h>
#include <stdbool.h>

#include <pthread.h>

#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif

struct event_fd;
struct event_timeout;
struct event_process; /* 进程 */

typedef void (*event_fd_handler)(struct event_fd *ev_fd,uint32_t events);
typedef void (*event_timeout_handler)(struct event_timeout *ev_tm);
typedef void (*event_process_handler)(struct event_process *ev_pro,int ret);

#define EVENT_READ              (1UL << 0)
#define EVENT_WRITE             (1UL << 1)
#define EVENT_EDGE_TRIGGER      (1UL << 2)
#define EVENT_BLOCKING          (1UL << 3)

#define EVENT_MASK              (EVENT_READ | EVENT_WRITE)

/**
 * @brief 监听文件描述符
 */
struct event_fd
{
    event_fd_handler cb;
    int fd;
    bool eof;
    bool error;
    bool registered; /*是否已经添加到epoll队列中*/
    uint8_t flags;
};

int event_fd_add(struct event_fd *ev_fd,uint32_t flags);
int event_fd_delete(struct event_fd *ev_fd);

/**
 * @brief 监听定时事件
 */
struct event_timeout
{
    struct list_head list;
    bool pending; /*添加一个timeout true 删除时false*/
    event_timeout_handler cb;
    struct timeval  time; /* 超时时间 */
};

int event_timeout_add(struct event_timeout *ev_tm);
int event_timeout_set(struct event_timeout *ev_tm,int msecs);
int event_timeout_cancel(struct event_timeout *ev_tm);
int event_timeout_remaining(struct event_timeout *ev_tm);



/**
 * @brief 初始化事件循环
 * @return
 */
int event_loop_init(void);

/**
 * @brief 事件循环入口
 */
void event_loop_run(void);

/**
 * @brief 销毁事件循环
 */
void event_loop_done(void);

void event_loop_cancel();



#ifdef __cplusplus
}
#endif

#endif
