#ifndef __NET_EVENT_H
#define __NET_EVENT_H

#include <stdint.h>
#include <stdbool.h>

#include "net_list.h"

#ifdef __cplusplus
extern "C" {
#endif

struct event_timeout;
struct event_process;
struct event_fd;
struct event_reactor;

typedef void (*event_fd_handler)(struct event_reactor *owner,struct event_fd *ev_fd,uint32_t events);
typedef void (*event_timeout_handler)(struct event_reactor *owner,struct event_timeout *ev_tm);
typedef void (*event_process_handler)(struct event_reactor *owner,struct event_process *ev_pro,int ret);

typedef struct event_fd *(*event_fd_malloc)(struct event_fd *ev_fd);
typedef void (*event_fd_free)(struct event_fd *ev_fd);

#define EVENT_READ                  (1UL << 0)
#define EVENT_WRITE                 (1UL << 1)
#define EVENT_EDGE_TRIGGER          (1UL << 2)
#define EVENT_BLOCKING              (1UL << 3)

#define EVENT_MASK      (EVENT_READ | EVENT_WRITE)

#define MAX_FD  32


/**
 * @brief 监听的文件描述符
 */
struct event_fd
{
    event_fd_handler h_fd;
    int fd;
    bool eof;
    bool error;
    bool registered; /* 是否已经添加到队列中*/
    uint8_t flags;
};

struct event_fd_event
{
    struct event_fd *fd;
    uint32_t events; /* event_read or event_write ...*/
};

struct event_reactor
{
    int listen_fd; /* epoll fd */
    uint32_t max_fd; /* epoll 最大监听文件数量*/
    struct list_head timeout_head;
    bool event_canclled;
    event_fd_free h_free;
    struct event_fd_event cur_fds[MAX_FD]; /*后面可改成队列*/
};

/**
 * @brief event_fd_add
 * @param ev_fd
 * @param flag 标志 event_read event_write EVENT_BLOCKING
 * @return
 */
int event_fd_add(struct event_reactor *reactor,struct event_fd *ev_fd,uint8_t flag);
int event_fd_delete(struct event_reactor *reactor,struct event_fd *ev_fd);

void event_clear_fds(struct event_reactor *reactor);

struct event_timeout
{
    struct list_head head;
    event_timeout_handler h_timeout;
    bool pending; /*添加一个timeout */
    struct timeval time; /*超时时间*/
};


int event_timeout_add(struct event_reactor *reactor,struct event_timeout *ev_tm);
int event_timeout_set(struct event_reactor *reactor,struct event_timeout *ev_tm,int msecs);
int event_timeout_cancel(struct event_timeout *ev_tm);
int event_timeout_remaining(struct event_timeout *ev_tm);
void event_clear_timeouts(struct event_reactor *reactor);


/**
 * @brief 初始化事件循环
 * @return
 */
struct event_reactor *event_loop_new(uint32_t max_fd);

/**
 * @brief 事件循环入口
 */
void event_loop_run(struct event_reactor *reactor);

/**
 * @brief 销毁事件循环
 */
void event_loop_delete(struct event_reactor *reactor);

void event_loop_cancel(struct event_reactor *reactor);

#ifdef __cplusplus
}
#endif

#endif
