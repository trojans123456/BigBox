#ifdef __linux__
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h> /* in glibc for getaddrinfo */
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#endif

int main(int argc,char *argv[])
{
    /* protocol 指明所要接收的协议包 0=protocol_ip
    AF_UNSPEC 返回的套接字地址可能为ipv4 or ipv6*/
   #if 0
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd < 0)
        return 0;
#endif
    /** host entry
     struct hostent
     {
        char *h_name; 主机名
        char **h_aliases; 域名列表 同一ip可绑定多个域名
        int h_addrtype; 主机ip地址的类型 ipv4 or ipv6
        int h_length; ip地址长度 ipv4 = 4byte ipv6=16byte
        //以网络字节序实现;需要inet_ntoa inet_ntop
        char **h_addr_list; 主机的ip地址 指向的可能是ipv4 or ipv6
     };
     gethostbyaddr gethostbyname 仅仅使用于 ipv4
     */

    /**
        struct addrinfo
        {
            int ai_flags;
            int ai_family; 可取的协议簇 AF_INET AF_UNIX
            int ai_socktype; SOCK_STREAM SOCK_RAW ...
            int ai_protocol; IPPROTO_IP TCP...
            socklen_t ai_addrlen; 地址长度
            struct sockaddr *ai_addr; ip地址
            char *ai_canonname;
            struct addrinfo *ai_next;
        };
     */
    struct addrinfo hint,*result;
    /* hostname = ipaddr service = port
     getaddrinfo(hostname,service,&hint,&result);
    */
	if(argc < 3)
		return 0;
    const char *hostname = argv[1];
    const char *service = argv[2];
    int ret = getaddrinfo(hostname,service,&hint,&result);
    if(ret != 0)
    {
        printf("%s \n",gai_strerror(ret));
        return 0;
    }

    int sockfd;
    struct addrinfo *info = NULL;
    for(info = result;info != NULL;info = info->ai_next)
    {
        sockfd = socket(info->ai_family,info->ai_socktype,info->ai_protocol);
        if(sockfd < 0)
            continue;

        /*
            struct sockaddr
            {
                sa_family_t sin_family;//地址族
                char sa_data[14]; //14字节，包含套接字中的目标地址和端口信息
            }
        */
        /**
          udp 中connect时将远程地址端口和fd绑定了所以可以用 send recv
          如果非connect则需要sendto recvfrom 带地址

          tcp的connect就是三次握手了
         */
        if(connect(sockfd,info->ai_addr,info->ai_addrlen) != -1)
            break;

        close(sockfd);
    }

    /* fail */
    if(!info)
    {
        close(sockfd);
        freeaddrinfo(result);
        return 0;
    }
    freeaddrinfo(result);

    /*
        bind();
    */
    char buffer[12] = "";
    int cnt = 0;
    while(1)
    {
        sprintf(buffer,"send## %d ",cnt++);
        send(sockfd,buffer,strlen(buffer)+1,0);

        sleep(1);
    }
}
