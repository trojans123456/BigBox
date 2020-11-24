#include <stdio.h>
#include "http.h"
#include "net.h"
#include "http_test.h"

void http_test_get()
{
    const char *url = "http://img-hxy021.didistatic.com/static/xcharge/do1_cP7sxkI327pSTyhixL9W";
    http_t *http = http_init(1024,1024);
    if(!http)
        return ;

    int ret = http_parser(url,&http->url);
    if(ret < 0)
    {
        printf("http parser failed\n");
        return ;
    }

    char buffer[256] = "";

    sprintf(buffer,"%s:%d",http->url.host,atoi(http->url.port));


    http_header_set_value(&http->reqeust.header_req_list,"Host",buffer);

    http_request_add_method(&http->reqeust,HTTP_REQUEST_METHOD_GET);
#if 0
    ret = http_connect(http,http->url.host,http->url.port);
    if(ret < 0)
    {
        printf("connect failed\n");
        return ;
    }

    http_send_request(http);
#endif
    IPAddress address;
    address.port = atoi(http->url.port);
    sock_dns_reverse(http->url.host,address.ip);

    printf("address = %s\n",address.ip);

    hSock sock = sock_tcp(NULL,2048);
    if(!sock)
    {
        printf("sock failed\n");
        return ;
    }
    ret = sock_connect(sock,&address);
    printf("ret = %d\n",ret);

    http_send_request(http);

    char data[2048] = "";
    int real_len = 0;
    int rv = 0;
    sleep(1);
    while(1)
    {
        real_len = sock_recv(sock,data + rv,256);
        printf("real_len  = %d\n",real_len);
        if(real_len <= 0)
            break;
        rv += real_len;
    }

    printf("%s\n",buffer);
}
