#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <stdbool.h>

#include "atcmd.h"

#define AT_RESP_END_OK                 "OK"
#define AT_RESP_END_ERROR              "ERROR"
#define AT_RESP_END_FAIL               "FAIL"
#define AT_END_CR_LF                   "\r\n"

#define AT_CLIENT_NUM_MAX   1
static struct at_client at_client_table[AT_CLIENT_NUM_MAX];

/* 在响应行缓冲区获取和解析响应缓冲区参数
Get one line AT response buffer by line number*/
const char *at_resp_get_line(at_response_t *resp, uint32_t resp_line)
{
    char *resp_buf = resp->buf;
    char *resp_line_buf = NULL;
    int line_num = 1;

    if(!resp)   return NULL;

    if (resp_line > resp->line_counts || resp_line <= 0)
    {

        return NULL;
    }

    for (line_num = 1; line_num <= resp->line_counts; line_num++)
    {
        if (resp_line == line_num)
        {
            resp_line_buf = resp_buf;

            return resp_line_buf;
        }

        resp_buf += strlen(resp_buf) + 1;
    }

    return NULL;
}
const char *at_resp_get_line_by_kw(at_response_t *resp, const char *keyword)
{
    char *resp_buf = resp->buf;
    char *resp_line_buf = NULL;
    int line_num = 1;

    if(!resp)   return NULL;
    if(!keyword)    return NULL;

    for (line_num = 1; line_num <= resp->line_counts; line_num++)
    {
        if (strstr(resp_buf, keyword))
        {
            resp_line_buf = resp_buf;

            return resp_line_buf;
        }

        resp_buf += strlen(resp_buf) + 1;
    }

    return NULL;
}
int at_resp_parse_line_args(at_response_t *resp, uint32_t resp_line, const char *resp_expr, ...)
{
    va_list args;
    int resp_args_num = 0;
    const char *resp_line_buf = NULL;

    if(!resp)   return -1;
    if(!resp_expr)  return -1;

    if ((resp_line_buf = at_resp_get_line(resp, resp_line)) == NULL)
    {
        return -1;
    }

    va_start(args, resp_expr);

    resp_args_num = vsscanf(resp_line_buf, resp_expr, args);

    va_end(args);

    return resp_args_num;
}

int at_resp_parse_line_args_by_kw(at_response_t *resp, const char *keyword, const char *resp_expr, ...)
{
    va_list args;
    int resp_args_num = 0;
    const char *resp_line_buf = NULL;

    if(!resp)   return -1;
    if(resp_expr)    return -1;

    if ((resp_line_buf = at_resp_get_line_by_kw(resp, keyword)) == NULL)
    {
        return -1;
    }

    va_start(args, resp_expr);

    resp_args_num = vsscanf(resp_line_buf, resp_expr, args);

    va_end(args);

    return resp_args_num;
}

static const struct at_urc *get_urc_obj(at_client_t *client)
{
    uint32_t i,prefix_len,suffix_len;
    uint32_t buf_sz;
    char *buffer = NULL;

    if(client->urc_table == NULL)
    {
        return NULL;
    }

    buffer = client->recv_buffer;
    buf_sz = client->recv_bufsz;

    for(i = 0;i < client->urc_table_size;i++)
    {
        prefix_len = strlen(client->urc_table[i].cmd_prefix);
        suffix_len = strlen(client->urc_table[i].cmd_suffix);

        if(buf_sz < prefix_len + suffix_len)
        {
            continue;
        }
        if ((prefix_len ? !strncmp(buffer, client->urc_table[i].cmd_prefix, prefix_len) : 1)
                && (suffix_len ? !strncmp(buffer + buf_sz - suffix_len, client->urc_table[i].cmd_suffix, suffix_len) : 1))
        {
            return &client->urc_table[i];
        }
    }

    return NULL;
}

static bool at_client_getchar(at_client_t *client,char *ch,int32_t timeout_ms)
{
    int result = true;
    struct timespec tv;


    while(read(client->fd,&ch,1) > 0)
    {
        if(timeout_ms <= 0)
        {
            result = sem_wait(&client->rx_notice);
            if(result != 0)
            {
                return false;
            }
        }
        else {
            tv.tv_sec = timeout_ms / 1000;
            tv.tv_nsec = (timeout_ms % 1000) * 1000000;
            result = sem_timedwait(&client->rx_notice,&tv);
            if(result != 0)
            {
                return false;
            }
        }


    }
    return true;
}

/**
 * @brief read line
 * @param client
 * @return
 */
static int at_recv_readline(at_client_t *client)
{
    int32_t read_len = 0;
    char ch = 0,last_ch = 0;
    bool is_full = false;

    memset(client->recv_buffer,0x00,client->recv_bufsz);
    client->cur_recv_len = 0;

    while(1)
    {
        at_client_getchar(client,&ch,0);
        if(read_len < client->recv_bufsz)
        {
            client->recv_buffer[read_len++] = ch;
            client->cur_recv_len = read_len;
        }
        else {
            is_full = true;
        }

        /* is newline or urc data */
        if((ch == '\n' && last_ch == '\r') || (client->end_sign != 0 && ch == client->end_sign)
            || get_urc_obj(client))
        {
            if(is_full)
            {
                memset(client->recv_buffer, 0x00, client->recv_bufsz);
                client->cur_recv_len = 0;
                return -1;
            }
            break;
        }
        last_ch = ch;
    }
    return read_len;
}


void *client_parser(void *args)
{
    uint32_t  resp_buf_len = 0;
    const struct at_urc *urc;
    uint8_t line_counts = 0;

    at_client_t *client = (at_client_t *)args;
    if(!client)
        return NULL;

    while(1)
    {
        if(at_recv_readline(client) > 0)
        {
            if((urc = get_urc_obj(client)) != NULL)
            {
                if(urc->func != NULL)
                {
                    urc->func(client->recv_buffer,client->recv_bufsz);
                }
            }
            else if(client->resp != NULL)
            {
                client->recv_buffer[client->cur_recv_len - 1] = '\0';
                if(resp_buf_len + client->cur_recv_len < client->resp->buf_size)
                {
                    /* copy response lines*/
                    memcpy(client->resp->buf + resp_buf_len,client->recv_buffer,client->cur_recv_len);
                    resp_buf_len += client->cur_recv_len;

                    line_counts++;
                }
                else {
                    client->resp_status = AT_RESP_BUFF_FULL;
                }

                /* check response result */
                if (memcmp(client->recv_buffer, AT_RESP_END_OK, strlen(AT_RESP_END_OK)) == 0
                        && client->resp->line_num == 0)
                {
                    /* get the end data by response result, return response state END_OK. */
                    client->resp_status = AT_RESP_OK;
                }
                else if (strstr(client->recv_buffer, AT_RESP_END_ERROR)
                         || (memcmp(client->recv_buffer, AT_RESP_END_FAIL, strlen(AT_RESP_END_FAIL)) == 0))
                {
                    client->resp_status = AT_RESP_ERROR;
                }
                else if (line_counts == client->resp->line_num && client->resp->line_num)
                {
                    /* get the end data by response line, return response state END_OK.*/
                    client->resp_status = AT_RESP_OK;
                }
                else
                {
                    continue;
                }
                client->resp->line_counts = line_counts;

                client->resp = NULL;
                sem_post(&client->resp_notice);
                resp_buf_len = 0, line_counts = 0;
            }
        }
    }
}

static int at_client_para_init(at_client_t *client)
{
    int ret = 0;
    client->status = AT_STATUS_UNINIT;
    client->cur_recv_len = 0;
    client->recv_buffer = (char *)calloc(1,sizeof(client->recv_bufsz));
    if(!client->recv_buffer)
    {
        return -1;
    }

    /* lock */
    pthread_mutex_init(&client->lock,NULL);
    /*sempore*/
    sem_init(&client->rx_notice,0,0);/* 0 pthread */
    sem_init(&client->resp_notice,0,0);

    client->urc_table = NULL;
    client->urc_table_size = 0;

    ret = pthread_create(&client->parser,NULL,client_parser,client);
    if(ret < 0)
    {
        sem_destroy(&client->resp_notice);
        sem_destroy(&client->rx_notice);
        pthread_mutex_destroy(&client->lock);
        free(client->recv_buffer);
        return -1;
    }
    return 0;
}

int at_client_init(const char *dev_name,uint32_t recv_bufsz)
{
   int idx = 0;

   at_client_t *client = NULL;

   for(idx = 0;idx < AT_CLIENT_NUM_MAX && at_client_table[idx].dev_name;idx++)
   {
       if(strcmp(at_client_table[idx].dev_name,dev_name) == 0)
       {
           break;
       }
   }

   if(idx >= AT_CLIENT_NUM_MAX)
   {
       return -1;
   }

    client = &at_client_table[idx];
    client->recv_bufsz = recv_bufsz;
    client->dev_name = strdup(dev_name);
    /* open device */

    return at_client_para_init(client);
}

at_client_t *at_client_get(const char *dev_name)
{
    int idx = 0;

    for(idx = 0;idx < AT_CLIENT_NUM_MAX;idx++)
    {
        if(strcmp(at_client_table[idx].dev_name,dev_name) == 0)
        {
            return &at_client_table[idx];
        }
    }
    return NULL;
}

at_client_t *at_client_get_first(void)
{
    if(strcmp(at_client_table[0].dev_name,"") == 0)
    {
        return NULL;
    }
    return &at_client_table[0];
}

at_response_t *at_create_resp(uint32_t buf_size, uint8_t line_num, uint32_t timeout)
{
    at_response_t *resp = NULL;

    resp = (at_response_t *) calloc(1, sizeof(struct at_response));
    if (resp == NULL)
    {
        return NULL;
    }

    resp->buf = (char *) calloc(1, buf_size);
    if (resp->buf == NULL)
    {
        free(resp);
        return NULL;
    }

    resp->buf_size = buf_size;
    resp->line_num = line_num;
    resp->line_counts = 0;
    resp->timeout = timeout;

    return resp;
}

void at_delete_resp(at_response_t *resp)
{
    if (resp && resp->buf)
    {
        free(resp->buf);
    }

    if (resp)
    {
        free(resp);
        resp = NULL;
    }
}

int at_client_obj_wait_connect(at_client_t *client,uint32_t timeout)
{
    int32_t result = 0;
    at_response_t *resp = NULL;
    time_t start_time = 0;

    if(client == NULL)
        return -1;

    resp = at_create_resp(16,0,500);
    if(!resp)
    {
        return -1;
    }

    pthread_mutex_lock(&client->lock);
    client->resp = resp;
    start_time = time(NULL);

    struct timespec tv;
    tv.tv_sec = resp->timeout / 1000;
    tv.tv_nsec = (resp->timeout % 1000) * 1000000;

    while(1)
    {
        if(time(NULL) - start_time > timeout)
        {
            result = -1;
            break;
        }

        resp->line_counts = 0;
        write(client->fd,"AT\r\n",4);
        if(sem_timedwait(&client->resp_notice,&tv) != 0)
            continue;
        else {
            break;
        }
    }

    at_delete_resp(resp);
    client->resp = NULL;

    pthread_mutex_unlock(&client->lock);
    return result;
}

int at_client_obj_send(at_client_t *client,const char *buf,uint32_t size)
{
    if(!client || size == 0)
        return -1;

    return write(client->fd,buf,size);
}

int at_client_obj_recv(at_client_t *client,char *buf, uint32_t size, int timeout)
{
    int read_idx = 0;
    bool result = 0;
    char ch;

    if(!client || !buf)
        return -1;

    while(1)
    {
        if(read_idx < size)
        {
            result = at_client_getchar(client,&ch,timeout);
            if(result == false)
            {
                return -1;
            }
            buf[read_idx++] = ch;
        }
        else {
            break;
        }
    }
    return read_idx;
}

void at_obj_set_end_sign(at_client_t *client,char ch)
{
    if(client)
    {
        client->end_sign = ch;
    }
}

void at_obj_set_urc_table(at_client_t *client,const at_urc_t *table,uint32_t table_sz)
{
    int idx;
    if(!client)
        return ;

    for(idx = 0;idx < table_sz;idx++)
    {
        if(table[idx].cmd_prefix == NULL ||
                table[idx].cmd_suffix == NULL)
            break;
    }

    client->urc_table = table;
    client->urc_table_size = table_sz;
}

int at_obj_exec_cmd(at_client_t *client,at_response_t *resp, const char *cmd_expr, ...)
{
    va_list args;
    int cmd_size = 0;
    int result = 0;
    const char *cmd = NULL;
    char send_buff[256] = "";

    if(!cmd_expr)   return -1;

    if (client == NULL)
    {

        return -1;
    }

    pthread_mutex_lock(&client->lock);

    client->resp_status = AT_RESP_OK;
    client->resp = resp;

    va_start(args, cmd_expr);
    int len = vsprintf(send_buff, cmd_expr, args);
    va_end(args);

    send_buff[len] = '\r';
    send_buff[len + 1] = '\n';

    write(client->fd,send_buff,len + 2);


    struct timespec tv;
    tv.tv_sec = resp->timeout / 1000;
    tv.tv_nsec = (resp->timeout % 1000) * 1000000;

    if (resp != NULL)
    {
        resp->line_counts = 0;
        /*信号量等待超时*/
        if (sem_timedwait(&client->resp_notice, &tv) != 0)
        {

            client->resp_status = AT_RESP_TIMEOUT;
            result = -1;
            goto __exit;
        }
        if (client->resp_status != AT_RESP_OK)
        {

            result = -1;
            goto __exit;
        }
    }

__exit:
    client->resp = NULL;

    pthread_mutex_unlock(&client->lock);

    return result;
}


at_response_t *at_resp_set_info(at_response_t *resp, uint32_t buf_size, uint8_t line_num, uint32_t timeout)
{
    if(!resp)   return NULL;

    if (resp->buf_size != buf_size)
    {
        resp->buf_size = buf_size;

        resp->buf = (char *) realloc(resp->buf, buf_size);
        if (!resp->buf)
        {

            return NULL;
        }
    }

    resp->line_num = line_num;
    resp->timeout = timeout;

    return resp;
}
