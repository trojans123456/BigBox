#ifndef __HTTP_H
#define __HTTP_H


#ifdef __cplusplus
extern "C" {
#endif

/* url parser */
struct http_url
{
    char *uri;					/* mandatory */
    char *scheme;               /* mandatory */
    char *host;                 /* mandatory */
    char *ip; 					/* mandatory */
    char *port;                 /* optional */
    char *path;                 /* optional */
    char *query;                /* optional */
    char *fragment;             /* optional */
    char *username;             /* optional */
    char *password;             /* optional */
};

/**
 * @brief 解析url
 * @param url
 * @return
 */
struct http_url *parser_url(const char *url);


/* http response*/
struct http_response
{
    struct http_url *request_uri;
    char *body;
    char *status_code;
    int status_code_int;
    char *status_text;
    char *request_headers;
    char *response_headers;
};


/* http request */
typedef enum http_req_type
{
    http_req_type_get = 0,
    http_req_type_options,
    http_req_type_head,
    http_req_type_post,
    http_req_type_put,
    http_req_type_delete,
    http_req_type_trace,
    http_req_type_connect,
    http_req_type_propfind,
    http_req_type_proppatch,
    http_req_type_mkcol,
    http_req_type_copy,
    http_req_type_move,
    http_req_type_lock,
    http_req_type_unlock
}http_req_type;

#define HTTP_HEADER_MAX 16
typedef struct http_header_list
{
    char *header[HTTP_HEADER_MAX];
    char *value[HTTP_HEADER_MAX];
}http_header_list;

int http_header_set_value(http_header_list *list,const char *name,const char *value);
char *http_header_get_value(http_header_list *list,const char *name);
int http_header_clear_value(http_header_list *list,const char *name);

struct http_request
{
    int method;
    float http_ver;
    char *host;
    char *resource;
    char *body;
    int body_len;
    http_header_list header_list;
};
struct http_request *http_request_new(void);
void http_request_delete(struct http_request *req);

/*no \r\n */
int http_request_add_method(struct http_request *req,int method);
int http_request_add_header(struct http_request *req,const char *name,const char *value);




#ifdef __cplusplus
}
#endif


#endif
