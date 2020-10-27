#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "net_socket.h"
#include "net_event.h"
#include "selector.h"

int ret,client_fd;

void handler(int sig)
{
    close_socket(ret);
    close_socket(client_fd);

    exit(1);
}

void client_opt(struct event_reactor *owner,struct event_fd *ev_fd,uint32_t events)
{
    if(ev_fd)
    {
        char buf[128] = "";
        printf("fd = %d\n",ev_fd->fd);
        readall(ev_fd->fd,buf,sizeof(buf));
        printf("buffer = %s\n",buf);
    }
}

void socket_handler(struct event_reactor *owner,struct event_fd *ev_fd,uint32_t events)
{
    if(!ev_fd)
        return ;
    int sockfd = accept_socket(ev_fd->fd,NULL,0,NULL,0);
    printf("sockfd = %d \n",sockfd);
    if(sockfd < 0)
        return ;

    struct event_fd *fd = (struct event_fd *)calloc(1,sizeof(struct event_fd));
    if(!fd)
        return ;

    fd->fd = sockfd;
    fd->h_fd = client_opt;

    event_fd_add(owner,fd,(EVENT_READ | EVENT_WRITE));
}

void socket_read(int fd,void *data)
{
    char buffer[128] = "";
    printf("fd = %d\n",fd);
    readall(fd,buffer,sizeof(buffer));
    printf("buffer = %s\n",buffer);
}

void accpet_handler(int fd,void *data)
{
    selector_t *sel = (selector_t *)data;
    if(!sel)
        return ;


    {
        int client_fd = accept_socket(fd,NULL,0,NULL,0);
        if(client_fd < 0)
            return ;

        printf("client_fd = %d\n",client_fd);
        set_nonblock(client_fd);
        sel_set_fd_handlers(sel,client_fd,NULL,socket_read,NULL,NULL);
        sel_set_fd_read_handler(sel,client_fd,SEL_FD_HANDLER_ENABLED);
    }
}

int main(int argc,char *argv[])
{
#if 0
    int ret = create_tcp_socket("172.23.233.133","6800",IP_TYPE_V4);
    if(ret < 0)
    {
        printf("create tcp failed\n");
        return -1;
    }

    while(1)
    {
        sendall(ret,"hongtao\n",sizeof("hongtao\n"));

        sleep(1);
    }
#endif
    signal(SIGINT,handler);



    ret = create_server_socket("192.168.0.129","6800",SOCKET_TYPE_TCP,IP_TYPE_V4,10);
    if(ret < 0)
    {
        printf("jslkfjds\n");
        return -1;
    }
    printf("ret = %d\n",ret);


#if 0
    selector_t *sel = NULL;
    sel_alloc_selector(&sel);
    if(!sel)
    {
        return 0;
    }
    sel_set_fd_handlers(sel,ret,sel,accpet_handler,NULL,NULL);
    sel_set_fd_read_handler(sel,ret,SEL_FD_HANDLER_ENABLED);

    sel_select_loop(sel);
#else
    struct event_reactor *reactor = event_loop_new(10);
    if(!reactor)
    {
        printf("create reactor failed\n");
        return -1;
    }

    struct event_fd sock_server;
    sock_server.fd = ret;
    sock_server.h_fd = socket_handler;


    event_fd_add(reactor,&sock_server,(EVENT_READ  | EVENT_BLOCKING));

    event_loop_run(reactor);
#endif
    return 0;
}
