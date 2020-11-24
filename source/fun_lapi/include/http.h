/**
  请求
--------------------------------------------------------
请求方法 | 空格 | URL | 空格 | 协议版本 | 回车换行|
-------------------------------------------------------
header | : | value | 回车换行 |
-----------------------------------------------------
\r\n
--------------------------------------------------
body
-------------------------------------------------

响应
-------------------------------------------------------
协议版本 | 空格 | 状态码 | 空格 | 状态描述符 | \r\n
-----------------------------------------------------
header | : | value | \r\n
-----------------------------------------
\r\n
---------------------------------------
body
----------------------------------------

*/

#ifndef __HTTP_H
#define __HTTP_H

#include "common.h"
#include "stringbuf.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum http_request_method
{
    HTTP_REQUEST_METHOD_GET,
    HTTP_REQUEST_METHOD_OPTIONS,
    HTTP_REQUEST_METHOD_HEAD,
    HTTP_REQUEST_METHOD_POST,
    HTTP_REQUEST_METHOD_PUT,
    HTTP_REQUEST_METHOD_DELETE,
    HTTP_REQUEST_METHOD_TRACE,
    HTTP_REQUEST_METHOD_CONNECT,
    HTTP_REQUEST_METHOD_PROPFIND,
    HTTP_REQUEST_METHOD_PROPPATCH,
    HTTP_REQUEST_METHOD_MKCOL,
    HTTP_REQUEST_METHOD_COPY,
    HTTP_REQUEST_METHOD_MOVE,
    HTTP_REQUEST_METHOD_LOCK,
    HTTP_REQUEST_METHOD_UNLOCK
}http_request_method_t;

#define HTTP_HEADER_MAX 16
typedef struct http_header_list
{
    char *header[HTTP_HEADER_MAX];
    char *value[HTTP_HEADER_MAX];
}http_header_list_t;

int http_header_set_value(http_header_list_t *list,const char *key,const char *value);
char *http_header_get_value(http_header_list_t *list,const char *key);
int http_header_del_value(http_header_list_t *list,const char *key);
int http_header_clr_value(http_header_list_t *list);

typedef struct http_request
{
    http_request_method_t method;
    float http_ver;
    http_header_list_t header_req_list;
    strbuf_t body;
}http_request_t;

int http_request_add_method(http_request_t *req, http_request_method_t method);
int http_request_add_body_context(http_request_t *req,const char *data);

typedef enum http_response_status_code
{
    HTTP_RESPONSE_STATUS_CODE_CONTINUE = 100,
    HTTP_RESPONSE_STATUS_CODE_OK = 200,
    HTTP_RESPONSE_STATUS_CODE_NOT_FOUND = 404
}http_response_status_code_t;

#define STATUS_DESC_MAX     64
typedef struct http_response
{
    http_response_status_code_t status_code;
    float http_ver;
    char status_desc[STATUS_DESC_MAX];
    http_header_list_t header_resp_list;
    strbuf_t body;
}http_response_t;

int http_response_add_status_code(http_response_t *resp,http_response_status_code_t status_code);
int http_response_add_body_context(http_response_t *resp,const char *data);
int http_response_add_status_desc(http_response_t *resp,const char *desc);

/*
* ①abc://②username:password@③example.com:④123⑤/path/data?⑥key=value&key2=value2#⑦fragid1
* ①scheme | ②user info | ③host | ④port | ⑤path | ⑥ query | ⑦fragment
*/

struct http_url
{
    char *uri;					/* 强制 */
    char *scheme;               /* 强制 */
    char *host;                 /* 强制 */
    char *ip; 					/* 强制 */
    char *port;                 /* 可选 */
    char *path;                 /* 可选 */
    char *query;                /* 可选 */
    char *fragment;             /* 可选 */
    char *username;             /* 可选 */
    char *password;             /* 可选 */
};

int http_parser(const char *data,struct http_url *url);

typedef struct http
{
    int fd;
    struct http_url url;
    http_request_t reqeust;
    http_response_t response;
}http_t;


http_t  *http_init(int req_bodysize, int resp_bodysize);


void http_release(http_t *http);

/**
 * @brief http_connect
 * @param http
 * @param hostname  ip或域名
 * @param service   服务或端口号
 * @return
 */
int http_connect(http_t *http,const char *hostname,const char *service);




typedef enum flag
{
    FLAGS_REQUEST,
    FLAGS_RESPONSE
}flag_t;

int http_send_request(http_t *http);
int http_send_response(http_t *http);



#ifdef __cplusplus
}
#endif

#endif
