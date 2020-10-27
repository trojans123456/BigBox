#ifndef __SELECTOR_H
#define __SELECTOR_H


#ifdef __cplusplus
extern "C" {
#endif

//#define FD_SETSIZE  64

/**
 * @brief 监听器结构体
 */
typedef struct selector__ selector_t;

typedef void (*sel_fd_handler_t)(int fd,void *data);

/**
 * @brief 定时器结构体
 */
typedef struct timer__  sel_timer_t;

typedef void (*sel_timeout_handler_t)(selector_t *a_this,
                                      sel_timer_t *timer,
                                      void *data);

typedef void (*sel_signal_handler_t)(void);

/**
 * @brief 分配一个定时器
 * @param sel
 * @param handler
 * @param user_data
 * @param new_timer 返回结果
 * @return
 */
int sel_alloc_timer(selector_t *sel,
                    sel_timeout_handler_t handler,
                    void *user_data,
                    sel_timer_t **new_timer);

/**
 * @brief 释放一个定时器
 * @param timer
 * @return
 */
int sel_free_timer(sel_timer_t *timer);

/**
 * @brief 启动一个定时器
 * @param timer
 * @param timeout
 * @return
 */
int sel_start_timer(sel_timer_t *timer,unsigned int timeout_ms);//struct timeval *timeout);

/**
 * @brief 停止一个定时器
 * @param timer
 * @return
 */
int sel_stop_timer(sel_timer_t *timer);


void sel_set_signal_handler(int sig,sel_signal_handler_t handler);

void sel_ignore_signal(int sig);

void setup_signals(void);



/**
 * @brief 分配一个select
 * @param new_selector 注意是二级指针
 * @return
 */
int sel_alloc_selector(selector_t **new_selector);

/**
 * @brief 释放一个select
 * @param new_selector
 * @return
 */
int sel_free_selector(selector_t *new_selector);


/**
 * @brief 设置该fd的读写异常处理函数
 * @param a_this
 * @param fd
 * @param user_data
 * @param read_handler
 * @param write_handler
 * @param except_handler
 */
void sel_set_fd_handlers(selector_t *a_this,
                         int fd,
                         void *user_data,
                         sel_fd_handler_t read_handler,
                         sel_fd_handler_t write_handler,
                         sel_fd_handler_t except_handler);

/**
 * @brief 删除该fd的处理函数，同时从监听队列中移除
 * @param a_this
 * @param fd
 */
void sel_clr_fd_handlers(selector_t *a_this,int fd);

#define SEL_FD_HANDLER_ENABLED  0
#define SEL_FD_HANDLER_DISABLED 1

/**
 * @brief 对fd设置是否监听和删除
 * @param a_this
 * @param fd
 * @param state
 */

void sel_set_fd_read_handler(selector_t *a_this,int fd,int state);
void sel_set_fd_write_handler(selector_t *a_this,int fd,int state);
void sel_set_fd_except_handler(selector_t *a_this,int fd,int state);

/**
 * @brief 仅运行一次监听器，然后返回
 * @param a_this
 */
void sel_select_once(selector_t *a_this);

/**
 * @brief 循环监听
 * @param a_this
 */
void sel_select_loop(selector_t *a_this);

#ifdef __cplusplus
}
#endif

#endif
