#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "http/e_http.h"


/*
* @brief 从url中解析端口号
* @params
*       str[in] 要解析的字符串
*       port[out] 解析出的端口号
*
*/
static void hander_port(const char *str,unsigned short *port)
{
    int i = 1;
    int tmp;
    int sum = 0;

    while(str[i])
    {
        if(str[i] >= '0' && str[i] <= '9' )
        {
            tmp = str[i] - '0';
            sum = sum * 10 + tmp;
            i++;
        }
    }
    *port = sum;
}

int url_parser(const char *url,url_t *output)
{
    int  ret = -1;
    char *tmp = NULL;
    char *port_index = NULL;

    if(NULL == url)
    {
        fprintf(stderr,"url is null!\n");
        return -1;
    }
    printf("---------url = %s \n",url);
    tmp = strstr(url,"http://");
    if(NULL == tmp)
    {
        fprintf(stderr,"url has no http:// header!\n");
        return -1;
    }

    /* 跳到http://后*/
    url += 7;
    tmp = NULL;
    tmp = strstr(url,"/");

    if(NULL == tmp)
    {
        fprintf(stderr,"url has no file index!\n");
        return -1;
    }
    snprintf(output->hostname,tmp-url + 1,"%s",url);

    /*用来解析是否有port*/
    port_index = strstr(output->hostname,":");

    if(port_index != NULL)
    {
        hander_port(port_index,&output->port);
    }

    sprintf(output->path,"%s",tmp);

    return 0;
}

int init_get_header(http_request_msg_t *header,char *message)
{
    int i = 0;
    const char *method = "GET "; /*注意空格*/
    const char *end = " HTTP/1.1\r\n";

    if(header == NULL)
    {
        fprintf(stderr,"no header!\n");
        return -1;
    }

    header->offset = 0;
    while(method[i])
    {
        header->request_buf[header->offset++] = method[i];
        i++;
    }

    i = 0;
    if(message != NULL)
    {
        while(message[i] && i < sizeof(header->request_buf))
        {
            header->request_buf[header->offset++] = message[i];
            i++;
        }
    }


    i = 0;
    while(end[i])
    {
        header->request_buf[header->offset++] = end[i];
        i++;
    }

    return 0;
}

int add_header_message(http_request_msg_t *header,const char *key,const char *value)
{
    int i = 0;

    if(NULL == header)
    {
        fprintf(stderr,"no request!\n");
        return -1;
    }

    while(key[i] != '\0' && i < sizeof(header->request_buf))
    {
        header->request_buf[header->offset++] = key[i];
        i++;
    }
    if((i + 2) < sizeof(header->request_buf))
    {
        header->request_buf[header->offset++] = ':';
        header->request_buf[header->offset++] = ' ';
    }

    i = 0;
    while(value[i] != '\0' && i < sizeof(header->request_buf))
    {
        header->request_buf[header->offset++] = value[i];
	//printf("value_bffer = %c \n",header->request_buf[header->offset]);
        i++;
    }

    if((i + 2) < sizeof(header->request_buf))
    {
        header->request_buf[header->offset++] = '\r';
        header->request_buf[header->offset++] = '\n';
    }

    //printf("buf = %s ,offset = %d , value = %s\n",header->request_buf,header->offset,value);

    return 0;
}

int end_request(http_request_msg_t *header)
{
    int i = 0;
    const char *end = "\r\n";

    if(NULL == header)
    {
        fprintf(stderr,"no request\n");
        return -1;
    }

    while(end[i] && i < sizeof(header->request_buf))
    {
        header->request_buf[header->offset++] = end[i];
        i++;
    }
    if((i + 1) < sizeof(header->request_buf))
        header->request_buf[header->offset] = '\0';

    return 0;
}

int get_response_status(const char *respond_buf)
{
    int code = 0;
    int i;
    int tmp;
    char *ptr = NULL;
    if((ptr = strstr(respond_buf,"HTTP/1.1")) == NULL)
    {
        fprintf(stderr,"respond_buf not find HTTP/1.1\n");
        return -1;
    }
    respond_buf += 9;// "http/1.1 "
    for(i = 0;i < 3;i++)
    {
        tmp = respond_buf[i] - '0';
        code = code * 10 + tmp;
    }

    return code;
}

int parser_response_header(const char *msg,const char *key,char *value)
{
    if(NULL == msg || NULL == key)
    {
        return -1;
    }

    const char *buf_ptr = msg;
    char *tmp_ptr = NULL;
    int count = 0;
    int pos = 0;
    char tmp_buf[128] = "";


    while((buf_ptr = strstr(buf_ptr + 2,"\r\n")) != NULL)
    {
        //printf("----buf_ptr = %s \n",buf_ptr);

        count = (buf_ptr - msg) - pos;
        if(count == 2)
        {
            break; /*\r\n\r\n*/
        }
        memset(tmp_buf,0x00,sizeof(tmp_buf));
        memcpy(tmp_buf,&msg[pos],count);

        pos += count + 2;/*\r\n*/

        tmp_ptr = strstr(tmp_buf,":");
        if(tmp_ptr)
        {
            if(strncmp(tmp_buf,key,tmp_ptr - tmp_buf) == 0)
            {
                memcpy(value,&tmp_buf[tmp_ptr - tmp_buf + 1],strlen(tmp_buf) - (tmp_ptr - tmp_buf + 1));

                return 0;
            }
        }

    }

    return -1;
}


/*去掉左右两边的指定字符*/
/*cfg = "\n\r\t" */
static char *strTim(char *val,char *cfg)
{
    if(NULL == val)
    {
        return NULL;
    }
    if(NULL == cfg)
    {
        return val;
    }

    int beg = -1,end = -1;
    int val_size = strlen(val);
    int cfg_size = strlen(cfg);

    unsigned int iS = 0;
    unsigned int iC = 0;
    for(iS = 0,iC = 0; iS < val_size; iS++)
    {
        for(iC = 0; iC < cfg_size && val[iS] != cfg[iC]; iC++)
        {
            /* nothing */
            ;
        }

        if(iC < cfg_size)
        {
            continue;
        }

        if(beg < 0)
        {
            beg = iS;
        }

        end = iS;
    }

    if(beg >= 0)
    {
        //printf("end = %d beg = %d\n",end,beg);
        val[end + 1] = '\0';
        return (val + beg);
    }
    return val; //return ""
}

enum content_type get_content_type(const char *msg)
{
    char buf[128] = "";
    if(parser_response_header(msg,"Content-Type",buf) != -1)
    {
        char *ptr = strTim(buf," ");
        /*有 xxx;xxx content-type:x-gzip;char-utf8*/
        if(strcmp(ptr,"application/x-gzip") == 0)
        {
            return CONTENT_TYPE_GZ;
        }
        if(strcmp(ptr,"application/octec-stream") == 0)
        {
            return CONTENT_TYPE_STREAM;
        }
    }

    return CONTENT_TYPE_UNKOWN;
};

long get_content_length(const char *msg)
{
   char buf[128] = "";
   if(parser_response_header(msg,"Content-Length",buf) != -1)
   {
       char *ptr = strTim(buf," ");
       return atol(ptr);
   }

   return -1;
}

int get_filename(const char *msg,char *filename)
{
    char buf[128] = "";
    if(parser_response_header(msg,"Content-Disposition",buf) != -1)
    {
        printf("find content-disposition \n");
        char *ptr = NULL;
        ptr = strstr(buf,"filename=");
        if(ptr)
        {
            char *name = strTim(ptr + strlen("filename="),"\"\r\n");
            printf("name = %s \n",name);

            strcpy(filename,name);
        }
    }
}

/************* post ****************/
int init_post_header(http_post_request_t *header,char *message)
{
    int i = 0;
    const char *method = "POST "; /*注意空格*/
    const char *end = " HTTP/1.1\r\n";

    if(header == NULL)
    {
        fprintf(stderr,"no header!\n");
        return -1;
    }

    header->request.offset = 0;
    while(method[i])
    {
        header->request.request_buf[header->request.offset++] = method[i];
        i++;
    }

    i = 0;
    if(message != NULL)
    {
        while(message[i] && i < sizeof(header->request.request_buf))
        {
            header->request.request_buf[header->request.offset++] = message[i];
            i++;
        }
    }


    i = 0;
    while(end[i])
    {
        header->request.request_buf[header->request.offset++] = end[i];
        i++;
    }

    return 0;
}

void make_post_boundary(http_post_request_t *request)
{
    if(request)
    {
        sprintf(request->boundary,"%s\r\n","-----------1234567890");
    }
}

int add_post_boundary(http_post_request_t *request)
{
    char buffer[128] = "";
    sprintf(buffer,"multipart/form-data; boundary=%s",request->boundary);
    add_header_message(&request->request,"Content-Type",buffer);

    request->request.request_buf[request->request.offset++] = '\r';
    request->request.request_buf[request->request.offset++] = '\n';

    return 0;
}

int add_post_content_params(http_post_request_t *request,const char *key,const char *value)
{
    int boundary_len = strlen(request->boundary);
    memcpy(&request->request.request_buf[request->request.offset],request->boundary,boundary_len);
    request->request.offset += boundary_len;

    //add header
    char buffer[128] = "";
    sprintf(buffer,"form-data; name=\"%s\"",key);
    add_header_message(&request->request,"Content-Disposition",buffer);

    request->request.request_buf[request->request.offset++] = '\r';
    request->request.request_buf[request->request.offset++] = '\n';

    //add value
    int len = strlen(value);
    memcpy(&request->request.request_buf[request->request.offset],value,len);
    request->request.offset += len;

    request->request.request_buf[request->request.offset++] = '\r';
    request->request.request_buf[request->request.offset++] = '\n';

    return 0;
}

void end_boundary(http_post_request_t *request)
{
    if(request)
    {
        int len = strlen(request->boundary);
        memcpy(&request->request.request_buf[request->request.offset],request->boundary,len);
        request->request.offset += len;

        request->request.request_buf[request->request.offset++] = '\r';
        request->request.request_buf[request->request.offset++] = '\n';
    }
}

/* multipart/form-data 上传文件 */
int add_post_file(http_post_request_t *request,const char *filename)
{
    char buffer[64] = "";
    snprintf(buffer,sizeof(buffer),"form-data; name=\"file\"; filename=\"%s\"",filename);
    //snprintf have a buf action!!!!!!!
#if 0
    char *ptr = strstr(buffer,filename);
    if(ptr)
    {
	printf("ptr = %s filename = %s \n",ptr,filename);
    	ptr += strlen(filename) + 2;
	*ptr = '\0';
    }
#endif
    add_header_message(&request->request,"Content-Disposition",buffer);

    add_header_message(&request->request,"Content-Type","text/plain");

    /* add file content */
    FILE *fp = fopen(filename,"r");
    if(NULL == fp)
    {
        return -1;
    }

    int cur_pos = request->request.offset;
    char value[1024] = "";
    int read_len = -1;

    while(request->request.offset < sizeof(request->request.request_buf) - cur_pos)
    {
        read_len = fread(value,sizeof(char),sizeof(value),fp);
	printf("read_len = %d %s\n",read_len,value);

        if(read_len > 0)
        {
            memcpy(&request->request.request_buf[request->request.offset],value,read_len);
	    printf("buf = %s \n",&request->request.request_buf[request->request.offset]);
            request->request.offset += read_len;
        }
	if(read_len <= 0)
		break;
    }
    return 0;
}

/***
POST / HTTP/1.1
Host: 172.23.233.54:6800
User-Agent: curl/7.47.0
Content-Length: 291
Expect: 100-continue
Content-Type: multipart/form-data; boundary=------------------------b276dab146c45dd9
\r\n
CLIENT->SERVER  172.23.233.54:53234 --------------------------b276dab146c45dd9
Content-Disposition: form-data; name="file"; filename="aaa.txt"
Content-Type: text/plain
\r\n
--------------------------b276dab146c45dd9
Content-Disposition: form-data; name="deviceid"
\r\n
TEST00056
-----------рц
CLIENT->SERVER  172.23.233.54:53234 ---------------b276dab146c45dd9-- \r\n
\r\n
*/
