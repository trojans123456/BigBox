#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netdb.h>

#include "http/e_http.h"

int connect_server(const char *addr,const char *port)
{
 /*struct addrinfo hints={0};*/
    struct addrinfo hints;
    memset(&hints,0,sizeof(struct addrinfo));

    hints.ai_family = AF_UNSPEC; /* IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* Must be TCP */
    int sockfd = -1;
    int rv;
    struct addrinfo *p, *servinfo;

    /* get address information */
    rv = getaddrinfo(addr, port, &hints, &servinfo);
    if(rv != 0) {
        fprintf(stderr, "Failed to open socket (getaddrinfo): %s\n", gai_strerror(rv));
        return -1;
    }

    /* open the first possible socket */
    int connectSucceed = 0;
    for(p = servinfo; p != NULL; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd < 0) continue;

        /* connect to server */
        rv = connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);

        if( 0 != rv )
        {
            close(sockfd);
            continue;
        }
        else
        {
            connectSucceed = 1;
            break;
        }
    }
    if(connectSucceed==0){
        return -1;
    }

    /* free servinfo */
    freeaddrinfo(servinfo);

    /* make non-blocking */
    if (sockfd != -1) fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL) | O_NONBLOCK);

    /* return the new socket fd */
    return sockfd;
}

void disconnect_server(int sockfd)
{
    close(sockfd);
}

ssize_t send_data(int sockfd,const char *buf,size_t len)
{
    size_t sent = 0;

    while(sent < len)
    {
        ssize_t tmp = send(sockfd, buf + sent, len - sent, 0);
        if (tmp < 1)
        {
            return 0;
        }
        sent += (size_t) tmp;
    }
    return sent;
}

ssize_t recv_data(int sockfd,char *buf,size_t bufsz)
{
    char *start = buf;
    ssize_t rv = 0;
    do
    {
        rv = recv(sockfd, buf, bufsz,0);

        if (rv > 0)
        {
            /* successfully read bytes from the socket */
            buf += rv;
            bufsz -= rv;
        }
        else if (rv < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
        {
            /* an error occurred that wasn't "nothing to read". */
            return 0;
        }

    } while (rv > 0);


    return buf - start;
}

int get_response_header(int sockfd,http_response_msg_t *respond)
{
    int i = 4;
    int ret;
    char *index = NULL;
    char flags = 1;

    /**/
 #if 1
    fd_set fds;
    struct timeval timeout = {3,0};

    while(i--)
    {
        FD_ZERO(&fds);
        FD_SET(sockfd,&fds);
        ret = select(sockfd + 1,&fds,NULL,NULL,&timeout);
        if(ret == 0)
        {
            continue;
        }
        else if(ret < 0)
        {
            return -1;
        }

        if(FD_ISSET(sockfd,&fds))
        {
            ret = recv_data(sockfd,respond->response_buf + respond->offset,1024);

            respond->offset += ret;
            printf("recv_: %s \n",respond->response_buf);

            index = strstr(respond->response_buf,"\r\n\r\n");//通过\r\n\r\n找到头末尾
            if(index)
            {
                printf("find respond content index!\n");
                flags = 2;
                break;
            }
        }

    }
#endif

    if(flags == 2)
    {
        respond->head_part_index = (index - respond->response_buf) + 4;
        respond->response_buf[respond->offset] = 0;
        return 0;
    }
    else
    {
        printf("not find header and end flags!\n");
        return -1;
    }

    return 0;
}

int get_file_info(int sockfd,http_response_msg_t *respond,url_t *url_msg,char *filename)
{
    http_request_msg_t request = {0};

    init_get_header(&request,url_msg->path);
    add_header_message(&request,"Host",url_msg->hostname); /*无port*/
    add_header_message(&request,"Accept","*/*");
    end_request(&request);

    printf("request: \n%s\n",request.request_buf);
    printf("request len: \n%d\n",request.offset);

    int ret = send_data(sockfd,request.request_buf,request.offset);
    if(ret < 0)
    {
        printf("send data failed\n");

        return -1;
    }

    /*获取响应*/
    ret = get_response_header(sockfd,respond);
    if(ret < 0)
    {
        printf("not found respond header\n");

        return -1;
    }

    /*获取响应状态*/
    int code = get_response_status(respond->response_buf);
    if(code != 200)
    {
        printf("respond error\n");

        return -1;
    }

    get_filename(respond->response_buf,filename);
    printf("get_filename = %s \n",filename);

    return 0;
}

FILE *init_local_file(http_response_msg_t *response,const char *fullpath)
{
    FILE *fd;
    int ret;
#if 0
    /*如果文件夹不存在fopen会出错,或fopen时创建*/
    if(access(HOME_WORK_PATH,F_OK) != 0)
    {
        printf("%s is not exist\n",HOME_WORK_PATH);
        mkdir(HOME_WORK_PATH,0775);
    }
#endif
    fd = fopen(fullpath,"wb");
    if(fd == NULL)
    {
        fprintf(stderr,"fopen err!\n");
        fprintf(stderr,"%s",strerror(errno));
        return NULL;
    }
    ret = fwrite(response->response_buf + response->head_part_index,sizeof(char),response->offset - response->head_part_index,fd);
    if(ret < 0)
    {
        fprintf(stderr,"fwrite err!\n");
        fclose(fd);
        return NULL;
    }

    return fd;
}

void download_file(int sockfd,FILE *fd)
{
    int ret = 0;
    char buf[4096];
    int w_ret;


    while(1)
    {
        fd_set read_fds;

        struct timeval timeout = {5,0};
        int maxfd = sockfd + 1;

        FD_ZERO(&read_fds);
        FD_SET(sockfd,&read_fds);


        ret = select(maxfd,&read_fds,NULL,NULL,&timeout);
        if(ret <= 0)
        {
            break;
        }


        if(FD_ISSET(sockfd,&read_fds))
        {
            ret = recv_data(sockfd,(char *)buf,sizeof(buf));
            w_ret = fwrite(buf,sizeof(char),ret,fd);
            if(w_ret < 0)
            {

                printf("write file errot %d\n", w_ret);
                break;
            }

        }
    }

    printf("download file over!\n");

    fflush(fd);

    fclose(fd);
    disconnect_server(sockfd);
}

int http_client(int argc,char *argv[])
{
//get
#if 0
    if(argc < 2)
    {
        printf("missing params argv[1]\n");
        return -1;
    }
    url_t url_msg;
    int ret = url_parser(argv[1],&url_msg);
    if(ret < 0)
    {
        printf("url parser failed\n");
        return -1;
    }

    if(url_msg.port == 0)
    {
        url_msg.port = 80;
    }

    char port[4] = "";
    sprintf(port,"%d",url_msg.port);
    int sockfd = connect_server(url_msg.hostname,port);
    if(sockfd < 0)
    {
        printf("connect server failed\n");
        return -1;
    }
    char filename[64] = "";
    http_response_msg_t response = {0};
    ret = get_file_info(sockfd,&response,&url_msg,filename);
    printf("filename = %s \n",filename);


    char fullpath[128] = "";
    sprintf(fullpath,"%s/%s",".",filename);
    FILE *fp = init_local_file(&response,fullpath);
    if(fp)
        download_file(sockfd,fp);
    else
        close(sockfd);

    return 0;
#endif
    char ipaddr[] = "192.168.1.11";
    char port[] = "6800";
    int sockfd = connect_server(ipaddr,port);
    if(sockfd < 0)
	    return -1;
    http_post_request_t post;

    init_post_header(&post,NULL);

    make_post_boundary(&post);

    add_header_message(&post.request,"Host","192.168.1.11:6800");

    printf("111111\n");
    add_post_boundary(&post);

    add_post_content_params(&post,"device","1234");
    printf("2222\n");
    add_post_file(&post,"test.txt");
    printf("333\n");
    end_boundary(&post);

    printf("%s ",post.request.request_buf);

    write(sockfd,post.request.request_buf,post.request.offset);

    while(1);
    close(sockfd);
}
