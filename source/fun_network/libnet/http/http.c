#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#include "http.h"

static int do_random(uint32_t seed)
{
    srand(seed);
    return rand();
}

int random_number(void)
{
    uint32_t seed = (uint32_t)time(NULL);
    static uint32_t last_seed = 1;
    last_seed += (seed >> ((seed ^ last_seed) % 3));
    return do_random(last_seed ^ seed);
}

char *random_string(uint32_t len)
{
    uint32_t i,flag,seed,random;

    char *str = (char *)calloc(1,len + 1);
    if(!str)
        return NULL;

    seed = (uint32_t)random_number();
    seed += (uint32_t)((unsigned int)(*str) ^ seed);

    random = (uint32_t)do_random(seed);

    for(i = 0;i < len;i++)
    {
        random = do_random(seed ^ random);
        flag = (uint32_t)random % 3;
        switch(flag)
        {
        case 0:
            str[i] = 'A' + do_random(random ^ (i & flag)) % 26;
            break;
        case 1:
            str[i] = 'a' + do_random(random ^ (i & flag)) % 26;
            break;
        case 2:
            str[i] = '0' + do_random(random ^ (i & flag)) % 10;
            break;
        default:
            str[i] = 'x';
            break;
        }
        random += ((0xb433e5c6 ^ random)  << (i & flag));
     }

    str[len] = '\0';
    return str;
}

http_url_t *http_url_new(void)
{
    http_url_t *url = NULL;
    url = (http_url_t *)malloc(sizeof(http_url_t));
    if(!url)
        return NULL;

    url->host = NULL;
    url->port = 80;
    url->proto = NULL;

    return url;
}

void http_url_delete(http_url_t *a_url)
{
    if(a_url)
    {
        if(a_url->host)
            free(a_url->host);
        if(a_url->port)
            free(a_url->proto);

        free(a_url);
    }
}

int http_url_parser(const char *a_string, http_url_t *a_uri)
{
#define parse_state_read_host   0
#define parse_state_read_port   1
#define parse_state_read_resource   2

    uint32_t l_state = parse_state_read_host;
    char *l_start_string = NULL;
    char *l_end_string = NULL;
    char  l_temp_port[6];

    /* init the array */
    memset(l_temp_port, 0, 6);
    /* check the parameters */
    if (a_string == NULL)
      goto ec;
#if 0
    if (a_uri) {
      a_uri->full = strdup(a_string);
    }
#endif
    l_start_string = strchr(a_string, ':');
    /* check to make sure that there was a : in the string */
    if (!l_start_string)
      goto ec;
    if (a_uri) {
      a_uri->proto = (char *)malloc(l_start_string - a_string + 1);
      memcpy(a_uri->proto, a_string, (l_start_string - a_string));
      a_uri->proto[l_start_string - a_string] = '\0';

      /* change default port for secure connections */
      if(!strcmp(a_uri->proto, "https")) {
        goto ec;
      }
    }
    /* check to make sure it starts with "http://" */
    if (strncmp(l_start_string, "://", 3) != 0)
      goto ec;
    /* start at the beginning of the string */
    l_start_string = l_end_string = &l_start_string[3];
    while(*l_end_string)
      {
        if (l_state == parse_state_read_host)
      {
        if (*l_end_string == ':')
          {
            l_state = parse_state_read_port;
            if ((l_end_string - l_start_string) == 0)
          goto ec;
            /* allocate space */
            if ((l_end_string - l_start_string) == 0)
          goto ec;
            /* only do this if a uri was passed in */
            if (a_uri)
          {
                    char *tmp = l_end_string;
                    while(*tmp != '/' && *tmp != 0) {
                            tmp++;
                    }
            a_uri->host = (char *)malloc(tmp - l_start_string + 1);
            /* copy the data */
            memcpy(a_uri->host, l_start_string, (tmp - l_start_string));
            /* terminate */
            a_uri->host[tmp - l_start_string] = '\0';
          }
            /* reset the counters */
            l_end_string++;
            l_start_string = l_end_string;
            continue;
          }
        else if (*l_end_string == '/')
          {
            l_state = parse_state_read_resource;
            if ((l_end_string - l_start_string) == 0)
          goto ec;
            if (a_uri)
          {
            a_uri->host = (char *)malloc(l_end_string - l_start_string + 1);
            memcpy(a_uri->host, l_start_string, (l_end_string - l_start_string));
            a_uri->host[l_end_string - l_start_string] = '\0';
          }
            l_start_string = l_end_string;
            continue;
          }
      }
        else if (l_state == parse_state_read_port)
      {
        if (*l_end_string == '/')
          {
            l_state = parse_state_read_resource;
            /* check to make sure we're not going to overflow */
            if (l_end_string - l_start_string > 5)
          goto ec;
            /* check to make sure there was a port */
            if ((l_end_string - l_start_string) == 0)
          goto ec;
            /* copy the port into a temp buffer */
            memcpy(l_temp_port, l_start_string, l_end_string - l_start_string);
            /* convert it. */
            if (a_uri)
          a_uri->port = atoi(l_temp_port);
            l_start_string = l_end_string;
            continue;
          }
        else if (isdigit(*l_end_string) == 0)
          {
            /* check to make sure they are just digits */
            goto ec;
          }
      }
        /* next.. */
        l_end_string++;
        continue;
      }

    if (l_state == parse_state_read_host)
      {
        if ((l_end_string - l_start_string) == 0)
      goto ec;
        if (a_uri)
      {
        a_uri->host = (char *)malloc(l_end_string - l_start_string + 1);
        memcpy(a_uri->host, l_start_string, (l_end_string - l_start_string));
        a_uri->host[l_end_string - l_start_string] = '\0';
        /* for a "/" */
        a_uri->resource = strdup("/");
      }
      }
    else if (l_state == parse_state_read_port)
      {
        if (strlen(l_start_string) == 0)
      /* oops.  that's not a valid number */
      goto ec;
        if (a_uri)
      {
        a_uri->port = atoi(l_start_string);
        a_uri->resource = strdup("/");
      }
      }
    else if (l_state == parse_state_read_resource)
      {
        if (strlen(l_start_string) == 0)
      {
        if (a_uri)
          a_uri->resource = strdup("/");
      }
        else
      {
        if (a_uri)
          a_uri->resource = strdup(l_start_string);
      }
      }
    else
      {
        /* uhh...how did we get here? */
        goto ec;
      }
    return 0;

   ec:
    return -1;
}

http_request_t *http_request_new()
{
    http_request_t *req = (http_request_t *)calloc(1,sizeof(http_request_t));
    if(!req)
        return NULL;

    req->req_msg.line = http_buffer_new(64);
    req->req_msg.header = http_buffer_new(64);
    req->req_msg.body = http_buffer_new(64);

    memset(req->protocol_version,0x00,sizeof(req->protocol_version));
    req->method = HTTP_REQ_METHOD_INVALID;

    return req;
}

void http_request_delete(http_request_t *req)
{
    if(req)
    {
        http_buffer_delete(req->req_msg.line);
        http_buffer_delete(req->req_msg.header);
        http_buffer_delete(req->req_msg.body);

        free(req);
    }
}

/* set version http/1.1 */
void http_request_set_version(http_request_t *req,const char *str)
{
    if(http_nmatch(str,"HTTP/",5) == 0)
    {
        sprintf(req->protocol_version,"%s",str);
    }
    else {
        sprintf(req->protocol_version,"HTTP/%s",str);
    }
}

void http_request_set_method(http_request_t *req,http_req_method_e method)
{
    req->method = method;
}

/* method /xxx */
static char *method_str[] =
{
    "INVALID",
    "GET",
    "POST"
};
void http_request_set_start_line(http_request_t *req,const char *path)
{
    if(!req)
        return ;

    http_buffer_append(req->req_msg.line,method_str[req->method]," ",path," ",req->protocol_version,HTTP_CRLF,NULL);
}

char *http_request_get_start_line(http_request_t *req)
{
    if(!req)
        return NULL;
    return http_get_data(req->req_msg.line);
}

uint32_t http_request_get_start_line_len(http_request_t *req)
{
    if(!req)
        return 0;
    return http_get_used(req->req_msg.line) - 2; /* 1 = '\0' 1 = 内部used */
}

void http_request_set_header(http_request_t *req,const char *key,const char *value)
{
    if(!req || !key || !value)
        return ;
    http_buffer_append(req->req_msg.header,key,":",value,HTTP_CRLF,NULL);
}

char *http_request_get_header_data(http_request_t *req)
{
    if(!req)
        return NULL;
    return http_get_data(req->req_msg.header);
}

char *http_request_get_header(http_request_t *req, const char *key)
{
    if(!req || !key)
        return NULL;
    int offset = strlen(key);
    char *data = http_get_data(req->req_msg.header);
    char *value = strstr(data,key);
    if(value)
        return (data + offset + 1); /* 1 = ':' */
    return NULL;
}

uint32_t http_request_get_header_data_len(http_request_t *req)
{
    if(!req)
        return 0;
    return http_get_used(req->req_msg.header) - 2;
}

void http_request_set_body(http_request_t *req,const char *buf,size_t size)
{
    if(!req || !buf)
        return ;

    http_buffer_cat(req->req_msg.body,buf,size);
}

char *http_request_get_body_data(http_request_t *req)
{
    if(!req)
        return NULL;
    return http_get_data(req->req_msg.body);
}

uint32_t http_request_get_msg_len(http_request_t *req)
{
    if(!req)
        return 0;

    uint32_t total = 0;
    total += http_get_used(req->req_msg.line);
    total += http_get_used(req->req_msg.header);
    total += http_get_used(req->req_msg.body);

    return total;
}

http_response_t *http_response_new(void)
{
    http_response_t *resp = (http_response_t *)calloc(1,sizeof(http_response_t));
    if(!resp)
        return NULL;


    return resp;
}

void http_response_delete(http_response_t *resp)
{
    if(resp)
    {
        if(resp->resp_msg.line)
            http_buffer_delete(resp->resp_msg.line);
        if(resp->resp_msg.resp_header)
            http_buffer_delete(resp->resp_msg.resp_header);
        if(resp->resp_msg.body)
            http_buffer_delete(resp->resp_msg.body);

        free(resp);
    }
}

void http_response_set_status_code(http_response_t *resp,http_resp_status_code_e code)
{
    if(resp)
    {
        resp->status_code = code;
    }
}

void http_response_set_version(http_response_t *resp,const char *str)
{
    if(resp)
    {
        if(http_nmatch(str,"HTTP/",5) == 0)
        {
            sprintf(resp->protocol_version,"%s",str);
        }
        else
        {
            sprintf(resp->protocol_version,"HTTP/%s",str);
        }
    }
}

void http_response_set_start_line(http_response_t *resp,const char *status_desc)
{
    buffer_append(resp->resp_msg.line,resp->protocol_version," ",resp->status_code," ",status_desc,HTTP_CRLF);
}

void http_response_set_header(http_response_t *resp,const char *key,const char *value)
{
    buffer_append(resp->resp_msg.resp_header,key,":",value,HTTP_CRLF);
}

char *http_response_get_header_data(http_response_t *resp)
{
    if(!resp)
        return NULL;

    return http_get_data(resp->resp_msg.resp_header);
}

void http_response_set_body(http_request_t *resp,const char *buf,size_t size)
{
    http_buffer_cat(resp->req_msg.body,buf,size);
}

char *http_response_get_body_data(http_request_t *resp)
{
    return http_get_data(resp->req_msg.body);
}







