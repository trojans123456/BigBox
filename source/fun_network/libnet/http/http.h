#ifndef __HTTP_H
#define __HTTP_H

#include <stdint.h>

#include "http_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
*request
*------------------------------------------------------------
* method | space | URL | space | protocol version | \r\n |
* -----------------------------------------------------------
* key | : | value | \r\n
* ........
* ------------------------------------------------------------
* \r\n
* ------------------------------------------------------------
* body
*-------------------------------------------------------------
*
* response
* ------------------------------------------------------------
* protocol version | space | status code | space | status code descri | \r\n
* ------------------------------------------------------------------------
* ....
* -----------------------
* \r\n
* ----------------
* body
* -----------------------
*/

#define HTTP_CRLF       "\r\n"

/**
 * @brief random_string  产生一个len长度的随机字符串 注意free释放
 * @param len
 * @return
 */
char *random_string(uint32_t len);

typedef struct
{
    char *host;
    char *proto;
    char *resource;
    uint16_t port;
}http_url_t;

http_url_t *http_url_new(void);
void http_url_delete(http_url_t *a_url);
int http_url_parser(const char *a_url,http_url_t *a_req);

typedef enum
{
    HTTP_REQ_METHOD_INVALID = 0x00,
    HTTP_REQ_METHOD_GET,
    HTTP_REQ_METHOD_POST
}http_req_method_e;


typedef struct
{
    http_req_method_e method;
    char protocol_version[16];
    struct http_req_msg
    {
        http_buffer_t *line;
        http_buffer_t *header;
        http_buffer_t *body;
    }req_msg;
}http_request_t;

http_request_t *http_request_new(void);
void http_request_delete(http_request_t *req);
/* set version http/1.1 */
void http_request_set_version(http_request_t *req,const char *str);
void http_request_set_method(http_request_t *req,http_req_method_e method);
/* method /xxx */
void http_request_set_start_line(http_request_t *req,const char *path);
char *http_request_get_start_line(http_request_t *req);
/* 不包括\0 */
uint32_t http_request_get_start_line_len(http_request_t *req);

void http_request_set_header(http_request_t *req,const char *key,const char *value);
char *http_request_get_header_data(http_request_t *req);
char *http_request_get_header(http_request_t *req,const char *key);
uint32_t http_request_get_header_data_len(http_request_t *req);

void http_request_set_body(http_request_t *req,const char *buf,size_t size);
char *http_request_get_body_data(http_request_t *req);

uint32_t http_request_get_msg_len(http_request_t *req);

typedef enum
{
    HTTP_RESP_STATUS_OK = 200,
    HTTP_RESP_STATUS_NOT_FOUND = 404
}http_resp_status_code_e;

/* 响应 */
typedef struct
{
    http_resp_status_code_e status_code;
    char protocol_version[16];
    struct http_resp_msg
    {
        http_buffer_t *line;
        http_buffer_t *resp_header;
        http_buffer_t *body;
    }resp_msg;
}http_response_t;

http_response_t *http_response_new(void);
void http_response_delete(http_response_t *resp);

void http_response_set_status_code(http_response_t *resp,http_resp_status_code_e code);
void http_response_set_version(http_response_t *resp,const char *str);

void http_response_set_start_line(http_response_t *resp,const char *status_desc);
void http_response_set_header(http_response_t *resp,const char *key,const char *value);
char *http_response_get_header_data(http_response_t *resp);

void http_response_set_body(http_request_t *resp, const char *buf, size_t size);
char *http_response_get_body_data(http_request_t *resp);

#ifdef __cplusplus
}
#endif

#endif
