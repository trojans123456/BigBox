#ifndef E_HTTP_H_
#define E_HTTP_H_

/*
*@brief url
* http://hostname[:port]/path/[;params][?query]#fragment
*/
typedef struct url
{
#define HOSTNAME_MAX_LEN    128
    char hostname[HOSTNAME_MAX_LEN];
#define PATH_MAX_LEN    128
    char path[PATH_MAX_LEN];
    unsigned short port;
}url_t;

/*
*@brief 请求头
*/
typedef struct
{
#define REQUEST_BUFFER_MAX_LEN  1024
    char request_buf[REQUEST_BUFFER_MAX_LEN];
    int offset;
}http_request_msg_t;

/*
*@brief 响应头
*/
typedef struct
{
#define RESPONSE_BUFFER_MAX_LEN 4096
    char response_buf[RESPONSE_BUFFER_MAX_LEN];
    int offset;
    int head_part_index;/* 头结束位置 */
}http_response_msg_t;

/** 解析 url */
int url_parser(const char *url,url_t *output);

/*
*@brief 初始化get请求头
*@params
    message 请求内容
*/
int init_get_header(http_request_msg_t *header,char *message);

/** 添加 头字段 host:111 */
int add_header_message(http_request_msg_t *header,const char *key,const char *value);

int end_request(http_request_msg_t *header);

/** 获取响应状态 200 400 500 */
int get_response_status(const char *msg);

/** 解析响应头 根据key  */
int parser_response_header(const char *msg,const char *key,char *value);

enum content_type
{
    CONTENT_TYPE_GZ,
    CONTENT_TYPE_STREAM,
    CONTENT_TYPE_UNKOWN
};
/* 获取内容类型 压缩包或数据流 */
enum content_type get_content_type(const char *msg);

/*获取文件长度 */
long get_content_length(const char *msg);

/*获取文件名 */
int get_filename(const char *msg,char *filename);


/** post **/
typedef struct
{
    http_request_msg_t request;
#define BOUNDARY_MAX_LEN    32
    char boundary[BOUNDARY_MAX_LEN];
}http_post_request_t;

int init_post_header(http_post_request_t *request,char *message);


void make_post_boundary(http_post_request_t *request);

int add_post_content_params(http_post_request_t *request,const char *key,const char *value);

/* multipart/form-data 上传文件 */
int add_post_file(http_post_request_t *request,const char *filename);

#endif // HTTP_H
