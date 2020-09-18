#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "atcmd.h"

#define AT_RESP_END_OK                 "OK"
#define AT_RESP_END_ERROR              "ERROR"
#define AT_RESP_END_FAIL               "FAIL"
#define AT_END_CR_LF                   "\r\n"

#define AT_CLIENT_NUM_MAX   1
static struct at_client at_client_table[AT_CLIENT_NUM_MAX] = {0};

at_response_t *at_create_resp(uint32_t buf_size, uint32_t line_num, uint32_t timeout)
{
    at_response_t *resp = NULL;

    resp = (at_response_t *)calloc(1,sizeof(at_response_t));
    if(!resp)
        return NULL;

    resp->buf = (char *)calloc(1,buf_size);
    if(!resp)
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
    if(resp && resp->buf)
    {
        free(resp->buf);

        free(resp);
        resp = NULL;
    }
}

at_response_t *at_resp_set_info(at_response_t *resp, uint32_t buf_size, uint32_t line_num, uint32_t timeout)
{
    if(!resp)
        return NULL;

    if(resp->buf_size != buf_size)
    {
        resp->buf_size = buf_size;
        resp->buf = (char *)realloc(resp->buf,buf_size);
        if(!resp->buf)
        {
            return NULL;
        }
    }

    resp->line_num = line_num;
    resp->timeout = timeout;

    return resp;
}

const char *at_resp_get_line(at_response_t *resp, uint32_t resp_line)
{
    char *resp_buf = resp->buf;
    char *resp_line_buf = NULL;
    uint32_t line_num = 1;

    if(!resp)
        return NULL;

    if(resp_line > resp->line_counts || resp_line <= 0)
    {
        return NULL;
    }

    for(line_num = 1;line_num <= resp->line_counts;line_num++)
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

int at_client_init(const char *dev_name, uint32_t recv_bufsz)
{
    int idx = 0;
    int result = 0;


}
