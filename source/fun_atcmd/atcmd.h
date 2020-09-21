#ifndef __ATCMD_H
#define __ATCMD_H

#include <stdint.h>
#include <stdarg.h>

#ifdef __linux__
#include <pthread.h>
#include <semaphore.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define AT_CMD_END_MARK_CRLF

#if defined(AT_CMD_END_MARK_CRLF)
#define AT_CMD_END_MARK             "\r\n"
#elif defined(AT_CMD_END_MARK_CR)
#define AT_CMD_END_MARK             "\r"
#elif defined(AT_CMD_END_MARK_LF)
#define AT_CMD_END_MARK             "\n"
#endif

typedef enum
{
    AT_STATUS_UNINIT = 0,
    AT_STATUS_INIT,
    AT_STATUS_BUSY
}at_status_e;

/********* client *************/
typedef enum
{
    AT_RESP_OK = 0,
    AT_RESP_ERROR = -1,
    AT_RESP_TIMEOUT = -2,
    AT_RESP_BUFF_FULL = -3
}at_resp_status_e;

typedef struct at_response
{

    uint32_t buf_size; /* 响应buffer最大字节数*/
    uint32_t timeout;
    uint8_t line_num;/* 0 == 收到 OK 或 ERROR返回; !=0 接收到line_num函数时返回*/
    uint8_t line_counts;/*接收到的行数*/
    /* response buffer */
    char *buf;
}at_response_t;

/**
  Unsolicited Result Code 客户端监听到服务端(芯片或协议)主动发送的AT数据
*/
typedef struct at_urc
{
    const char *cmd_prefix;
    const char *cmd_suffix;
    void (*func)(const char *data,uint32_t size);
}at_urc_t;

typedef struct at_client
{
    at_status_e status;
    char end_sign;
    char *recv_buffer;
    uint32_t recv_bufsz;
    uint32_t cur_recv_len;
    sem_t rx_notice;
    pthread_mutex_t lock;
    at_response_t *resp;
    sem_t resp_notice;
    at_resp_status_e resp_status;

    const at_urc_t *urc_table;
    uint32_t urc_table_size;
    const char *dev_name;
    int fd;
    pthread_t parser;
}at_client_t;

/**
 * @brief 初始化
 * @param dev_name
 * @param recv_bufsz
 * @return
 */
int at_client_init(const char *dev_name,uint32_t recv_bufsz);

/* get client object */
at_client_t *at_client_get(const char *dev_name);
at_client_t *at_client_get_first(void);

/**
 * @brief 等待确认设备是否初始化成功
 * @param client
 * @param timeout
 * @return
 */
int at_client_obj_wait_connect(at_client_t *client,uint32_t timeout);

int at_client_obj_send(at_client_t *client,const char *buf,uint32_t size);

int at_client_obj_recv(at_client_t *client,char *buf, uint32_t size, int timeout);

void at_obj_set_end_sign(at_client_t *client,char ch);

void at_obj_set_urc_table(at_client_t *client, const at_urc_t *table, uint32_t table_sz);

/**
 * @brief 发送AT指令
 * @param client
 * @param resp
 * @param cmd_expr
 * @return
 */
int at_obj_exec_cmd(at_client_t *client, at_response_t *resp, const char *cmd_expr, ...);


/**
 * @brief 创建一个响应
 * @param buf_size
 * @param line_num
 * @param timeout
 * @return
 */
at_response_t *at_create_resp(uint32_t buf_size, uint8_t line_num, uint32_t timeout);


void at_delete_resp(at_response_t *resp);


at_response_t *at_resp_set_info(at_response_t *resp, uint32_t buf_size, uint8_t line_num, uint32_t timeout);

/* 在响应行缓冲区获取和解析响应缓冲区参数 */
const char *at_resp_get_line(at_response_t *resp, uint32_t resp_line);
const char *at_resp_get_line_by_kw(at_response_t *resp, const char *keyword);
int at_resp_parse_line_args(at_response_t *resp, uint32_t resp_line, const char *resp_expr, ...);
int at_resp_parse_line_args_by_kw(at_response_t *resp, const char *keyword, const char *resp_expr, ...);

/* ========================== single AT client function ============================ */
/**
  如果只有一个客户端可以直接使用, 多个客户端时需要在初始化函数前使用
*/
#define at_exec_cmd(resp, ...)                   at_obj_exec_cmd(at_client_get_first(), resp, __VA_ARGS__)
/*等待模块初始化完成*/
#define at_client_wait_connect(timeout)          at_client_obj_wait_connect(at_client_get_first(), timeout)
/*发送指定长度数据*/
#define at_client_send(buf, size)                at_client_obj_send(at_client_get_first(), buf, size)
/*接收指定长度数据*/
#define at_client_recv(buf, size, timeout)       at_client_obj_recv(at_client_get_first(), buf, size, timeout)
/*设置接收数据的行结束符*/
#define at_set_end_sign(ch)                      at_obj_set_end_sign(at_client_get_first(), ch)
#define at_set_urc_table(urc_table, table_sz)    at_obj_set_urc_table(at_client_get_first(), urc_table, table_sz)


#ifdef __cplusplus
}
#endif

#endif
