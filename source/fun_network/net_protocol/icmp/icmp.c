#include "icmp.h"
#include <stdio.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <net/if_arp.h>
#include <netinet/ip_icmp.h>
#include <sys/time.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

static unsigned short cal_chksum(unsigned short *addr,int len)
{
    int nleft=len;
    int sum=0;
    unsigned short *w=addr;
    unsigned short answer=0;

    while(nleft > 1)
    {
        sum += *w++;
        nleft -= 2;
    }
    if( nleft == 1)
    {
        *(unsigned char *)(&answer) = *(unsigned char *)w;
        sum += answer;
    }
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;

    return answer;
}

int net_ping(const char *ip,unsigned int port,const int count,const int packsize)
{
    char sendpacket[128];
    char recvpacket[128];

    int send_cnt = 0;
    int recv_cnt = 0;

    char ipstr[64];
#if 1
    struct hostent *p = gethostbyname(ip);
    if(p ==NULL)
    {
        printf("get hosy by name fail\n");
        return -1;
    }
    if(p->h_addrtype == AF_INET || p->h_addrtype == AF_INET6)
    {
        if (inet_ntop(p->h_addrtype,*(p->h_addr_list),ipstr,sizeof(ipstr)) == NULL)
        {
            printf("get hosy by name to ipaddr fail\n");
            return -1;
        }
    }
#endif

    struct sockaddr_in addr;
    bzero(&addr,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ipstr);

    // 取得socket
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0)
    {
        printf("create socket fail\n");
        return -1;
    }

    // 设定TimeOut时间
    struct timeval *tval;
    struct timeval timeo;
    //timeo.tv_sec = timeout / 1000;
    //timeo.tv_usec = timeout % 1000;
   // if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeo, sizeof(timeo)) == -1)
   // {
   //     return -1;
   // }

    // 设定Ping包
    memset(sendpacket, 0, sizeof(sendpacket));

    // 取得PID，作为Ping的Sequence ID
    pid_t pid=getpid();


    struct icmp *icmp;
    icmp=(struct icmp*)sendpacket;
    icmp->icmp_type=ICMP_ECHO;
    icmp->icmp_code=0;
    icmp->icmp_cksum=0;
    icmp->icmp_seq=0;
    icmp->icmp_id=pid;
    //psize=8+packsize;
    tval = (struct timeval *)icmp->icmp_data;
    gettimeofday(tval,NULL);
    int psize = 8 + packsize;
    icmp->icmp_cksum=cal_chksum((unsigned short *)icmp,psize);

    printf("now send pakcage----\n");
    // 发包
    int i;
    for(i=0; i<count; i++)
    {
        int n = sendto(sockfd, (char *)&sendpacket, psize, 0, (struct sockaddr *)&addr, sizeof(addr));
        if (n < 0)
        {
            printf("sendto fail errno:%s\n",strerror(errno));
            continue;
        }
        else
        {
            send_cnt++;
        }

        // 接受
        int maxfds = 0;
        fd_set readfds;


        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        maxfds = sockfd + 1;

        timeo.tv_sec = 2000 / 1000;
        timeo.tv_usec = 2000 % 1000;

        int nl = select(maxfds, &readfds, NULL, NULL, &timeo);
        if (nl <= 0)
        {
            printf("select fail\n");
            continue;
        }

        // 接受
        memset(recvpacket, 0, sizeof(recvpacket));
        struct sockaddr_in from;
        socklen_t fromlen = sizeof(from);
        n = recvfrom(sockfd, recvpacket, sizeof(recvpacket), 0, (struct sockaddr *)&from, &fromlen);
        if (n < 1)
        {
            continue;
        }

        // 判断是否是自己Ping的回复
        char *from_ip = (char *)inet_ntoa(from.sin_addr);

        if (strcmp(from_ip,ipstr) != 0)
        {
            continue;
        }

        struct ip *iph;
        iph = (struct ip *)recvpacket;
        icmp=(struct icmp *)(recvpacket + (iph->ip_hl<<2));

        // 判断Ping回复包的状态
        if (icmp->icmp_type == ICMP_ECHOREPLY && icmp->icmp_id == pid)
        {
            printf("recv packeg pid = %d\n",pid);
            recv_cnt++;
            continue;
        }
    }

    close(sockfd);

    printf("recv_cnt = %d send_cnt = %d \n",recv_cnt,send_cnt);
    if(recv_cnt > (send_cnt % 2))
    {
        printf("recv_cnt = %d send_cnt = %d \n",recv_cnt,send_cnt);
        return 1;
    }


    return -1;
}
