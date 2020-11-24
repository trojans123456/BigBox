#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <strings.h>

#ifdef __linux__
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#include "http.h"

/*检查字符串是否合法*/
static int is_scheme_char(int c)
{
     return (!isalpha(c) && '+' != c && '-' != c && '.' != c) ? 0 : 1;
}

static char *hostname2ip(char *hostname)
{
    struct hostent *h;
    if((h = gethostbyname(hostname)) == NULL)
    {
        return NULL;
    }
    return inet_ntoa(*(struct in_addr *)h->h_addr);
}


static char *http_strdup_key(const char *str)
{
    int len = strlen(str);
    char *val = (char *)malloc(len + 2); /* 1 : 1 \0*/
    if(!val)
        return NULL;
    snprintf(val,len + 1,"%s:",str);
    return val;
}

static char *http_strdup_val(const char *str)
{
    int len = strlen(str);
    char *val = (char *)malloc(len + 3); /* 2 \r\n + 1 \0 */
    if(!val)
        return NULL;
    snprintf(val,len + 2,"%s\r\n",str);
    return val;
}
#if 0
static char *http_strdup(const char *str)
{
    int len = strlen(str);
    char *val = (char *)malloc(len + 1);
    if(!val)
        return NULL;
    snprintf(val,len + 1,"%s",str);
    return val;
}
#endif
int http_header_set_value(http_header_list_t *list,const char *key,const char *value)
{
    if(!list || !key || !value)
        return -1;
    int l_return = 0;
    int i;
    char *l_temp_value = NULL;
    l_temp_value = http_header_get_value(list,key);
    if(l_temp_value == NULL) /*没有对应的头 */
    {
        for(i = 0;i < HTTP_HEADER_MAX;i++)
        {
            if(list->header[i] == NULL)
            {
                list->header[i] = http_strdup_key(key);
                list->value[i] = http_strdup_val(key);
                l_return = 1;
                break;
            }
        }
    }
    else /*有对应的头*/
    {
        for(i = 0;i < HTTP_HEADER_MAX;i++)
        {
            /* 找到该头*/
            if(strcasecmp(list->value[i],l_temp_value) == 0)
            {
                free(list->value[i]);
                list->value[i] = http_strdup_val(value);
                l_return = 1;
                break;
            }
        }
    }
    return l_return;
}

char *http_header_get_value(http_header_list_t *list,const char *key)
{
    int i = 0;
    char *l_return = NULL;
    for(i = 0;i < HTTP_HEADER_MAX;i++)
    {
        if(list->header[i] &&
           (strcasecmp(list->header[i],key) == 0))
        {
            if(list->value[i] == NULL)
                return NULL;
            l_return = list->value[i];
            break;
        }
    }
    return l_return;
}

int http_header_del_value(http_header_list_t *list,const char *key)
{
    int i = 0;
    int l_return =0;
    if(!list || !key)
        return -1;

    for(i = 0;i < HTTP_HEADER_MAX;i++)
    {
        if(list->header[i] &&
           (strcasecmp(list->header[i],key) == 0))
        {
            free(list->header[i]);
            list->header[i] = NULL;

            free(list->value[i]);
            list->value[i] = NULL;

            l_return =1;
            break;
        }
    }

    return l_return;
}

int http_header_clr_value(http_header_list_t *list)
{
    int i = 0;
    if(!list)
        return -1;
    for(i = 0;i < HTTP_HEADER_MAX;i++)
    {
        if(list->header[i])
            free(list->header[i]);
        list->header[i] = NULL;

        if(list->value[i])
            free(list->value[i]);
        list->value[i] = NULL;
    }
    return 0;
}


int http_request_add_method(http_request_t *req,http_request_method_t method)
{
    if(!req)
        return -1;
    req->method = method;
    return 0;
}

int http_request_add_body_context(http_request_t *req,const char *data)
{
    if(!req)
        return -1;
    return req->body.append(&req->body,data);
}


int http_response_add_status_code(http_response_t *resp,http_response_status_code_t status_code)
{
    if(!resp)
        return -1;
    resp->status_code = status_code;
    return 0;
}

int http_response_add_body_context(http_response_t *resp,const char *data)
{
    if(!resp || !data)
        return -1;

    return resp->body.append(&resp->body,data);
}

int http_response_add_status_desc(http_response_t *resp,const char *desc)
{
    if(!resp || !desc)
        return -1;
    snprintf(&resp->status_desc[0],sizeof(resp->status_desc),"%s",desc);
    return 0;
}


static void free_url(struct http_url *url)
{
    if(!url)
    {
        if(!url->scheme)    free(url->scheme);
        if(!url->host)      free(url->host);
        if(!url->port)      free(url->port);
        if(!url->path)      free(url->path);
        if(!url->query)     free(url->query);
        if(!url->fragment)  free(url->fragment);
        if(!url->username)  free(url->username);
        if(!url->password)  free(url->password);
    }
}

int http_parser(const char *data,struct http_url *url)
{
    const char *tmpstr = NULL;
    const char *curstr = NULL;
    int len;
    int i;
    int userpass_flag;
    int bracket_flag;

    if(!data || !url)
        return -1;
    curstr = data;

    /*
     * <scheme>:<scheme-specific-part>
     *
    */
    /* read scheme */
    tmpstr = strchr(curstr,':');
    if(NULL == tmpstr)
    {
        printf("11111111\n");
        return -1;
    }

    /* get the scheme length */
    len = tmpstr - curstr;
    /* check restrictions */
    for(i = 0;i < len;i++)
    {
        if(is_scheme_char(curstr[i]) == 0)
        {
            printf("22222222\n");
            return -1;
        }
    }

    /* copy the scheme to the storage */
    url->scheme = (char *)malloc(sizeof(char) * (len + 1));
    if(!url->scheme)
    {
        free_url(url);
        printf("33333333\n");
        return -1;
    }
    strncpy(url->scheme,curstr,len);
    url->scheme[len] = '\0';

    /* make the character to lower if it is upper case */
    for(i = 0;i < len;i++)
    {
        url->scheme[i] = tolower(url->scheme[i]);
    }

    /* skip ':' */
    tmpstr++;
    curstr = tmpstr;

    /*
        * //<user>:<password>@<host>:<port>/<url-path>
        * Any ":", "@" and "/" must be encoded.
        */
       /* Eat "//" */
    for(i = 0;i < 2;i++)
    {
        if('/' != *curstr)
        {
            free_url(url);
            printf("5555555\n");
            return -1;
        }
        curstr++;
    }

    /* check if te user(and password) are specified */
    userpass_flag = 0;
    tmpstr = curstr;
    while('\0' != *tmpstr)
    {
        if('@' == *tmpstr)
        {
            /* username and password are specified */
            userpass_flag = 1;
            break;
        }
        else if('/' == *tmpstr)
        {
            /* end of <host>:<port> specification */
            userpass_flag = 0;
            break;
        }
        tmpstr++;
    }

    /* user and password specification */
    tmpstr = curstr;
    if(userpass_flag)
    {
        /* read username */
        while('\0' != *tmpstr && ':' != *tmpstr && '@' != *tmpstr)
        {
            tmpstr++;
        }
        len = tmpstr - curstr;
        url->username = (char *)malloc(sizeof(char) * (len + 1));
        if(url->username == NULL)
        {
            free_url(url);
            printf("666666666\n");
            return -1;
        }
        strncpy(url->username,curstr,len);
        url->username[len] = '\0';

        /* proceed current pointer */
        curstr = tmpstr;
        if(':' == *curstr)
        {
            /* skip ':'*/
            curstr++;
            /* read password */
            tmpstr = curstr;
            while('\0' != *tmpstr && '@' != *tmpstr )
            {
                tmpstr++;
            }
            len = tmpstr - curstr;
            url->password = (char *)malloc(sizeof(char) * (len + 1));
            if(!url->password)
            {
                free_url(url);
                printf("777777777\n");
                return -1;
            }
            strncpy(url->password,curstr,len);
            url->password[len] = '\0';
            curstr = tmpstr;
        }

        /* skip '@' */
        if('@' != *curstr)
        {
            free_url(url);
            printf("888888888\n");
            return -1;
        }
        curstr++;
    }

    if('[' == *curstr)
    {
        bracket_flag = 1;
    }
    else
    {
        bracket_flag = 0;
    }

    /* proceed on by delimiters with reading host */
    tmpstr = curstr;
    while('\0' != *tmpstr)
    {
        if(bracket_flag && ']' == *tmpstr)
        {
            /* end of ipv6 address */
            tmpstr++;
            break;
        }
        else if(!bracket_flag && (':' == *tmpstr || '/' == *tmpstr))
        {
            /* port number is specified */
            break;
        }
        tmpstr++;
    }
    len = tmpstr - curstr;
    url->host = (char *)malloc(sizeof(char) * (len + 1));
    if(!url->host)
    {
        free_url(url);
        printf("999999999\n");
        return -1;
    }
    strncpy(url->host,curstr,len);
    url->host[len] = '\0';
    curstr = tmpstr;

    /* is port number specified ? */
    if(':' == *curstr)
    {
        curstr++;
        /* read port number */
        tmpstr = curstr;
        while('\0' != *tmpstr && '/' != *tmpstr)
        {
            tmpstr++;
        }
        len = tmpstr - curstr;
        url->port = (char *)malloc(sizeof(char) * (len + 1));
        if(!url->port)
        {
            free_url(url);
            printf("aaaaaaa\n");
            return -1;
        }
        strncpy(url->port,curstr,len);
        url->port[len] = '\0';
        curstr = tmpstr;
    }
    else
    {
        url->port = "80";
    }

    /* get ip */
    char *ip = hostname2ip(url->host);
    url->ip = ip;

    /* set uri */
    url->uri = (char *)url;

    /* end of the string */
    if('\0' == *curstr)
    {
        return 0;
    }

    /* skip '/' */
    if('/' != *curstr)
    {
        free_url(url);
        printf("bbbbbb\n");
        return -1;
    }
    curstr++;

    /* parser path */
    tmpstr = curstr;
    while('\0' != *tmpstr && '#' != *tmpstr)
    {
        tmpstr++;
    }
    len = tmpstr - curstr;
    url->path = (char *)malloc(sizeof(char) * (len + 1));
    if(!url->path)
    {
        free_url(url);
        printf("cccccc\n");
        return -1;
    }
    strncpy(url->path,curstr,len);
    url->path[len] = '\0';
    curstr = tmpstr;

    /* is query specified */
    if( '?' == *curstr)
    {
        /* Skip '?' */
        curstr++;
        /* Read query */
        tmpstr = curstr;
        while ( '\0' != *tmpstr && '#' != *tmpstr )
        {
            tmpstr++;
        }
        len = tmpstr - curstr;
        url->query = (char*)malloc(sizeof(char) * (len + 1));
        if ( NULL == url->query )
        {
            free_url(url);
            printf("ddddddd\n");
            return -1;
        }
        (void)strncpy(url->query, curstr, len);
        url->query[len] = '\0';
        curstr = tmpstr;
    }

    /* Is fragment specified? */
    if ( '#' == *curstr )
    {
        /* Skip '#' */
        curstr++;
        /* Read fragment */
        tmpstr = curstr;
        while ( '\0' != *tmpstr )
        {
            tmpstr++;
        }
        len = tmpstr - curstr;
        url->fragment = (char*)malloc(sizeof(char) * (len + 1));
        if ( NULL == url->fragment )
        {
            free_url(url);
            printf("eeeeee\n");
            return -1;
        }
        (void)strncpy(url->fragment, curstr, len);
        url->fragment[len] = '\0';
        curstr = tmpstr;
    }

    return 0;
}


http_t *http_init(int req_bodysize,int resp_bodysize)
{
    http_t *http = NULL;
    if(req_bodysize <= 0 || resp_bodysize <= 0)
        return NULL;

    http = (http_t *)calloc(1,sizeof(http_t));
    if(!http)
        return NULL;

    http->fd = -1;
    int i;
    for(i = 0;i < HTTP_HEADER_MAX;i++)
    {
        http->reqeust.header_req_list.header[i] = NULL;
        http->reqeust.header_req_list.value[i] = NULL;
    }

    http->reqeust.http_ver = 1.1;
    strbuf_init(&http->reqeust.body,req_bodysize);

    for(i = 0;i < HTTP_HEADER_MAX;i++)
    {
        http->response.header_resp_list.header[i] = NULL;
        http->response.header_resp_list.value[i] = NULL;
    }

    memset(&http->response.status_desc,0,sizeof(http->response.status_desc));
    http->response.http_ver = 1.1;
    strbuf_init(&http->response.body,resp_bodysize);

    return http;
}

void http_release(http_t *http)
{
    if(http)
    {
        if(http->fd > 0)
            close(http->fd);

        http_header_clr_value(&http->reqeust.header_req_list);
        http_header_clr_value(&http->response.header_resp_list);



        strbuf_release(&http->reqeust.body);
        strbuf_release(&http->response.body);

        free(http);
    }
}

const char *http_req_type_char[] =
{
    "GET",
    "OPTIONS",
    "HEAD",
    "POST",
    "PUT",
    "DELETE",
    "TRACE",
    "CONNECT",
    "PROPFIND",
    "PROPPATCH",
    "MKCOL",
    "COPY",
    "MOVE",
    "LOCK",
    "UNLOCK",
    NULL
};

int http_send_request(http_t *http)
{
    if(!http || http->fd < 0)
        return -1;

    int pos = 0;
    char *buffer = (char *)calloc(1,sizeof(http->reqeust.body.len) + 1024);
    if(!buffer)
        return -1;

    int len = sprintf(buffer + pos,"%s ",http_req_type_char[http->reqeust.method]);
    pos += len;

    len = sprintf(buffer + pos,"%s ",http->url.path ? http->url.path : "/");
    pos += len;

    len = sprintf(buffer + pos,"HTTP/%f\r\n",http->reqeust.http_ver);
    pos += len;

    int i;
    for(i = 0;i < HTTP_HEADER_MAX;i++)
    {
        if(http->reqeust.header_req_list.value[i] &&
           http->reqeust.header_req_list.header[i])
        {
            len = sprintf(buffer + pos,"%s%s\r\n",http->reqeust.header_req_list.header[i],http->reqeust.header_req_list.value[i]);
            pos += len;
        }
    }

    len = sprintf(buffer + pos,"%s","\r\n");
    pos += len;

    /* body */
    memmove(buffer + pos,http->reqeust.body.buf,http->reqeust.body.len);
    pos += http->reqeust.body.len;
    http->reqeust.body.consume(&http->reqeust.body,http->reqeust.body.len);

    int rv = 0;
    int st = 0;
    while(st < pos)
    {
        rv = send(http->fd,buffer + st,pos - st,0);
        if(rv < 1)
            return -1;
        st += rv;
    }

    free(buffer);

    return 0;
}

int http_send_response(http_t *http)
{
    if(!http)
        return -1;

    int pos = 0;
    char *buffer = (char *)calloc(1,sizeof(http->response.body.len) + 1024);
    if(!buffer)
        return -1;

    int len = sprintf(buffer + pos,"HTTP/%f ",http->response.http_ver);
    pos += len;

    len = sprintf(buffer + pos,"%d %s\r\n",http->response.status_code,http->response.status_desc);
    pos += len;

    int i;
    for(i = 0;i < HTTP_HEADER_MAX;i++)
    {
        if(http->response.header_resp_list.header[i] &&
           http->response.header_resp_list.value[i])
        {
            len = sprintf(buffer + pos,"%s%s\r\n",http->response.header_resp_list.header[i],http->response.header_resp_list.value[i]);
            pos += len;
        }
    }

    len = sprintf(buffer + pos,"%s","\r\n");
    pos += len;

    memmove(buffer + pos,http->response.body.buf,http->response.body.len);
    pos += http->response.body.len;

    http->response.body.consume(&http->response.body,http->response.body.len);

    int rv = 0;
    int st = 0;
    while(st < pos)
    {
        rv = send(http->fd,buffer + st,pos - st,0);
        if(rv < 1)
            return -1;
        st += rv;
    }

    free(buffer);

    return 0;
}


int http_connect(http_t *http, const char *hostname, const char *service)
{
    if(!http || !hostname || !service)
        return -1;

    int sockfd = -1;
    int ret = -1;
    const char *err_str;
    struct addrinfo hint,*result,*addrinfo;


    memset(&hint,0,sizeof(hint));

    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM; // tcp

    if((ret = getaddrinfo(hostname,service,&hint,&result)) != 0)
    {
        err_str = gai_strerror(ret);
        printf("%s \n",err_str);
        return -1;
    }

    for(addrinfo = result;addrinfo != NULL;addrinfo = addrinfo->ai_next)
    {
        http->fd = socket(addrinfo->ai_family,addrinfo->ai_socktype,addrinfo->ai_protocol);
        if(sockfd < 0)
            continue;

        if(connect(http->fd,addrinfo->ai_addr,addrinfo->ai_addrlen) != -1)
            break;

        close(http->fd);
    }

    if(!addrinfo)
    {
        close(http->fd);
        freeaddrinfo(result);
        return -1;
    }

    freeaddrinfo(result);

    return 0;
}



