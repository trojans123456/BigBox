#include <fcntl.h>
#include <unistd.h>
#include <net/if.h>
#include <strings.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <net/if_arp.h>
#include <netinet/ip_icmp.h>
#include <sys/time.h>

#include "net_interface.h"

int net_set_nonblock(int sockfd)
{
    int flags = 0;
    flags = fcntl(sockfd,F_GETFL,0);
    if(flags == -1)
        return -1;

    if(fcntl(sockfd,F_SETFL,flags | O_NONBLOCK) < 0)
        return -1;
    return 1;
}

int net_get_ipaddr(const char *devname,char *result)
{
    if(!devname || !result)
        return -1;

    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd < 0)
        return -1;

    struct ifreq ifr;
    bzero(&ifr,sizeof(struct ifreq));
    strcpy(ifr.ifr_name,devname);

    if(ioctl(sockfd,SIOCGIFADDR,&ifr) < 0)
    {
        close(sockfd);
        return -1;
    }


    struct sockaddr_in *pAddr;
    pAddr = (struct sockaddr_in *)&(ifr.ifr_addr);

    char addr[64] = "";
    const char *ptr = inet_ntop(AF_INET,&(pAddr->sin_addr),addr,sizeof(addr));
    if(ptr == NULL)
    {
        close(sockfd);
        return -1;
    }

    close(sockfd);
    strncpy(result,addr,sizeof(addr));

    return 1;
}

int net_get_macaddr(const char *devname,char *result)
{
    if(!devname || !result)
        return -1;

    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd < 0)
        return -1;

    struct ifreq ifr;
    bzero(&ifr,sizeof(struct ifreq));
    strcpy(ifr.ifr_name,devname);

    if(ioctl(sockfd,SIOCGIFHWADDR,&ifr) < 0)
    {
        close(sockfd);
        return -1;
    }


    sprintf(result,"%02x:%02x:%02x:%02x:%02x:%02x",(unsigned char)ifr.ifr_hwaddr.sa_data[0],\
            (unsigned char)ifr.ifr_hwaddr.sa_data[1],\
            (unsigned char)ifr.ifr_hwaddr.sa_data[2],\
            (unsigned char)ifr.ifr_hwaddr.sa_data[3],\
            (unsigned char)ifr.ifr_hwaddr.sa_data[4],\
            (unsigned char)ifr.ifr_hwaddr.sa_data[5]);

    close(sockfd);
    return 1;
}

int net_get_nettype(const char *devname)
{
    if(!devname)
        return NET_TYPE_NONE;

    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd < 0)
        return NET_TYPE_NONE;

    struct ifreq ifr;
    bzero(&ifr,sizeof(struct ifreq));
    strcpy(ifr.ifr_name,devname);

    if(ioctl(sockfd,SIOCGIFHWADDR,&ifr) < 0)
    {
        close(sockfd);
        return NET_TYPE_NONE;
    }


    int ret = NET_TYPE_NONE;
    switch(ifr.ifr_hwaddr.sa_family)
    {
    case ARPHRD_ETHER:
        ret = NET_TYPE_ETH;
        break;
    case ARPHRD_PPP:
        ret = NET_TYPE_4G;
        break;
    case ARPHRD_LOOPBACK:
        ret = NET_TYPE_LOOP;
        break;
    default:
        ret = NET_TYPE_NONE;
    }

    close(sockfd);
    return ret;
}

int net_get_destaddr(const char *devname,char *result)
{
    if(!devname || !result)
        return -1;

    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd < 0)
        return -1;

    struct ifreq ifr;
    memset(&ifr,0,sizeof(ifr));
    strcpy(ifr.ifr_name,devname);

    if(ioctl(sockfd,SIOCGIFDSTADDR,&ifr) < 0)
    {
        close(sockfd);
        return -1;
    }


    struct sockaddr_in *pAddr;
    pAddr = (struct sockaddr_in *)&(ifr.ifr_addr);
    char addr[64] = "";

    const char *ptr = inet_ntop(AF_INET,&(pAddr->sin_addr),addr,sizeof(addr));
    if(ptr == NULL)
    {
        close(sockfd);
        return -1;
    }

    close(sockfd);
    strncpy(result,addr,sizeof(addr));

    return 1;
}

int net_get_mask(const char *devname,char *result)
{
    if(!devname || !result)
        return -1;

    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd < 0)
        return -1;

    struct sockaddr_in *pAddr;
    struct ifreq ifr;

    memset(&ifr,0,sizeof(ifr));
    strcpy(ifr.ifr_name,devname);

    if(ioctl(sockfd,SIOCGIFNETMASK,&ifr) < 0)
    {
        close(sockfd);
        return -1;
    }


    pAddr = (struct sockaddr_in *)&(ifr.ifr_addr);
    char buf[64] = "";
    const char *ptr = inet_ntop(AF_INET,&(pAddr->sin_addr),buf,sizeof(buf));
    if(ptr == NULL)
    {
        close(sockfd);
        return -1;
    }

    close(sockfd);
    strncpy(result,buf,sizeof(buf));

    return 1;
}

int net_get_ipbydomain(const char *domain,char *result)
{
    struct hostent *pHost = NULL;
    int j = 0;
    char *pszTemp = NULL;
    unsigned int uIP = 0;

    pHost=gethostbyname(domain);
    if(pHost == NULL)
    {
        return -1;
    }

    pszTemp = (char*)&uIP;
    for(j=0; j<1&&NULL!=*(pHost->h_addr_list); pHost->h_addr_list++,j++)
    {
        memcpy(pszTemp,*(pHost->h_addr_list),pHost->h_length);
        break;
    }

    /**/
    char buf[64] = "";
    const char *ptr = inet_ntop(AF_INET,&uIP,buf,sizeof(buf));
    if(ptr == NULL)
    {
        return -1;
    }

    strncpy(result,buf,sizeof(buf));

    return 1;
}
/**check net name is or not exist.. ppp0 **/
int net_check_netname(const char *devname)
{
    DEBUG_("devname = %s \n",devname);
    if(!devname)
        return -1;

    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd < 0)
        return -1;

    struct ifconf ifc;
    struct ifreq ifr[10];
    ifc.ifc_len = sizeof(struct ifreq) * 10;
    ifc.ifc_buf = (char *)ifr;

    int ret = ioctl(sockfd,SIOCGIFCONF,(char *)&ifc);
    if(ret < 0)
    {
        close(sockfd);
        return -1;
    }


    int if_count = ifc.ifc_len / (sizeof(struct ifreq));
    int i;
    for(i = 0; i < if_count; i++)
    {
        DEBUG_("get  net-name = %s \n",ifr[i].ifr_name);
        if(strcmp(ifr[i].ifr_name,devname) == 0)
        {
            close(sockfd);
            return 1;
        }

    }

    close(sockfd);
    return -1;
}

int net_access(const char *name)
{
    if(!name)
        return -1;

    if(access(name,F_OK) == 0)
        return 1;

    return -1;
}

int net_test_constatus(const char *ipaddr,unsigned int port)
{
    if(!ipaddr || port > 9999)
        return -1;

    char ip_addr[64] = "";
    int ret = net_get_ipbydomain(ipaddr,ip_addr);
    if(ret < 0)
        return -1;

    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd < 0)
        return -1;

    struct sockaddr_in addr;
    bzero(&addr,sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip_addr);
    addr.sin_port = htons(port);

    net_set_nonblock(sockfd);

    connect(sockfd,(struct sockaddr *)&addr,sizeof(addr));
    int now_errno = errno;
    if(now_errno != EINPROGRESS)
    {
        close(sockfd);
        return -1;
    }

    int res;
    fd_set fdr,fdw;
    struct timeval timeout;
    FD_ZERO(&fdr);
    FD_ZERO(&fdw);
    FD_SET(sockfd,&fdr);
    FD_SET(sockfd,&fdw);

    timeout.tv_sec = 2;
    timeout.tv_usec = 0;

    res = select(sockfd + 1,&fdr,&fdw,NULL,&timeout);
    if(res <= 0)
    {
        close(sockfd);
        return -1;
    }
    if(res == 1)
    {
        if(FD_ISSET(sockfd,&fdw))
        {
            close(sockfd);
            return 1;
        }
    }

    if(res == 2)
    {
        close(sockfd);
        return -1;
    }

    close(sockfd);
    return 1;
}

int net_interface_up(const char *devname)
{
    if(!devname)
        return -1;

    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd < 0)
        return -1;

    struct ifreq ifr;
    strcpy(ifr.ifr_name,devname);
    short flag;
    flag = IFF_UP;

    if(ioctl(sockfd,SIOCGIFFLAGS,&ifr) < 0)
    {
        close(sockfd);
        return -1;
    }


    ifr.ifr_ifru.ifru_flags |= flag;
    if(ioctl(sockfd,SIOCSIFFLAGS,&ifr) < 0)
    {
        close(sockfd);
        return -1;
    }


    close(sockfd);
    return 1;
}

int net_interface_down(const char *devname)
{
    if(!devname)
        return -1;

    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd < 0)
        return -1;

    struct ifreq ifr;
    strcpy(ifr.ifr_name,devname);
    short flag;
    flag = ~IFF_UP;

    if(ioctl(sockfd,SIOCGIFFLAGS,&ifr) < 0)
    {
        close(sockfd);
        return -1;
    }

    ifr.ifr_ifru.ifru_flags &= flag;
    if(ioctl(sockfd,SIOCSIFFLAGS,&ifr) < 0)
    {
        close(sockfd);
        return -1;
    }

    close(sockfd);
    return 1;
}

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
    DEBUG_("ping----------------- %s \n",ip);
    char sendpacket[128];
    char recvpacket[128];

    int send_cnt = 0;
    int recv_cnt = 0;

    char ipstr[64];
#if 0
    struct hostent *p = gethostbyname(ip);
    if(p ==NULL)
    {
        DEBUG_("get hosy by name fail\n");
        return -1;
    }
    if(p->h_addrtype == AF_INET || p->h_addrtype == AF_INET6)
    {
        if (inet_ntop(p->h_addrtype,*(p->h_addr_list),ipstr,sizeof(ipstr)) == NULL)
        {
            DEBUG_("get hosy by name to ipaddr fail\n");
            return -1;
        }
    }
#endif
    int ret = net_get_ipbydomain(ip,ipstr);
    if(ret < 0)
    {
        DEBUG_("get ip by domain fail\n");
        return -1;
    }
    struct sockaddr_in addr;
    bzero(&addr,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ipstr);

    // 取得socket
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0)
    {
        DEBUG_("create socket fail\n");
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

    DEBUG_("now send pakcage----\n");
    // 发包
    int i;
    for(i=0; i<count; i++)
    {
        int n = sendto(sockfd, (char *)&sendpacket, psize, 0, (struct sockaddr *)&addr, sizeof(addr));
        if (n < 0)
        {
            DEBUG_("sendto fail errno:%s\n",strerror(errno));
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
            DEBUG_("select fail\n");
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
            DEBUG_("recv packeg pid = %d\n",pid);
            recv_cnt++;
            continue;
        }
    }

    close(sockfd);

    DEBUG_("recv_cnt = %d send_cnt = %d \n",recv_cnt,send_cnt);
    if(recv_cnt > (send_cnt % 2))
    {
        DEBUG_("recv_cnt = %d send_cnt = %d \n",recv_cnt,send_cnt);
        return 1;
    }


    return -1;
}

