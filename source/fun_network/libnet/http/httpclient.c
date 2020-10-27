#include <stdio.h>
#include "net_socket.h"
#include "selector.h"
#include "http.h"

int main(int argc,char *argv[])
{
    int sockfd = create_tcp_socket("172.23.233.44","6800",IP_TYPE_V4);
    if(sockfd < 0)
    {
        printf("create sockfd failed\n");
        return 0;
    }

    http_request_t *req = http_request_new();
    if(!req)
    {
        goto error;
    }

    http_request_set_method(req,HTTP_REQ_METHOD_GET);
    http_request_set_version(req,"1.1");
    http_request_set_start_line(req,"/");

    char host[64] = "";
    sprintf(host,"%s:%d","172.23.233.44",6800);
    http_request_set_header(req,"Host",host);

    unsigned int total = http_request_get_msg_len(req);

    char *send_data = (char *)malloc(total + 10);
    if(!send_data)
        goto error;
    uint32_t pos = 0;
    uint32_t len = http_request_get_start_line_len(req);
    memcpy(send_data + pos,http_request_get_start_line(req),len);
    pos += len;
    printf("Pos = %d\n",pos);

    memcpy(send_data + pos  ,http_request_get_header_data(req),http_get_used(req->req_msg.header));
    pos += http_get_used(req->req_msg.header);

    printf("--- %s\n",send_data);

    writeall(sockfd,send_data,pos);

    error:
    if(sockfd > 0)
        close(sockfd);
    if(req)
        http_request_delete(req);

    return 0;
}
