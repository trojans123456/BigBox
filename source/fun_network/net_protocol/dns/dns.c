#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<unistd.h>

#include "dns.h"

/*
**从www.baidu.com转换到3www5baidu3com
*/
#if 1
void ChangetoDnsNameFormat(unsigned char* dns, unsigned char* host) {
    int lock = 0, i;
    strcat((char*) host, ".");
    for (i = 0; i < strlen((char*) host); i++) {
        if (host[i] == '.') {
            *dns++ = i - lock;
            for (; lock < i; lock++) {
                *dns++ = host[lock];
            }
            lock++;
        }
    }
    *dns++ = '\0';
    printf("dns = %s \n",dns);
}
#endif 
/*
**实现DNS查询功能
*/
void dns_parser(unsigned char *host, unsigned char *ipaddr) {
    printf("---------------");
    unsigned char buf[65536], *qname, *reader;
    int i, j, stop, s;

    struct sockaddr_in a;//地址

    struct RES_RECORD answers[20], auth[20], addit[20];//回答区域、授权区域、附加区域中的资源数据字段
    struct sockaddr_in dest;//地址

    struct DNS_HEADER *dns = NULL;
    struct QUESTION *qinfo = NULL;

    printf("\n所需解析域名：%s\n", host);

    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); //建立分配UDP套结字

    dest.sin_family = AF_INET;//IPv4
    dest.sin_port = htons(53);//53号端口
    dest.sin_addr.s_addr = inet_addr(DNS_SERVER_IPADDR);//DNS服务器IP

    dns = (struct DNS_HEADER *) &buf;
    /*设置DNS报文首部*/
    dns->id = (unsigned short) htons(getpid());//id设为进程标识符
    dns->qr = 0; //查询
    dns->opcode = 0; //标准查询
    dns->aa = 0; //不授权回答
    dns->tc = 0; //不可截断
    dns->rd = 1; //期望递归
    dns->ra = 0; //不可用递归
    dns->z = 0; //必须为0
    dns->ad = 0;
    dns->cd = 0;
    dns->rcode = 0;//没有差错
    dns->q_count = htons(1); //1个问题
    dns->ans_count = 0;
    dns->auth_count = 0;
    dns->add_count = 0;

    //qname指向查询问题区域的查询名字段
    qname = (unsigned char*) &buf[sizeof(struct DNS_HEADER)];

    ChangetoDnsNameFormat(qname, host);//修改域名格式
    qinfo = (struct QUESTION*) &buf[sizeof(struct DNS_HEADER)
    + (strlen((const char*) qname) + 1)]; //qinfo指向问题查询区域的查询类型字段

    qinfo->qtype = htons(A); //查询类型为A
    qinfo->qclass = htons(1); //查询类为1

    //向DNS服务器发送DNS请求报文
    printf("\n\n发送报文中...");
    if (sendto(s, (char*) buf,sizeof(struct DNS_HEADER) + (strlen((const char*) qname) + 1)+ sizeof(struct QUESTION), 0, (struct sockaddr*) &dest,sizeof(dest)) < 0)
    {
        perror("发送失败！");
    }
    printf("发送成功！");

    //从DNS服务器接受DNS响应报文
    i = sizeof dest;
    printf("\n接收报文中...");
    if (recvfrom(s, (char*) buf, 65536, 0, (struct sockaddr*) &dest,(socklen_t*) &i) < 0) {
        perror("接收失败！");
    }
    printf("接收成功！");

    dns = (struct DNS_HEADER*) buf;

    //将reader指向接收报文的回答区域
    reader = &buf[sizeof(struct DNS_HEADER) + (strlen((const char*) qname) + 1)
            + sizeof(struct QUESTION)];

    printf("\n\n响应报文包含: ");
    printf("\n %d个问题", ntohs(dns->q_count));
    printf("\n %d个回答", ntohs(dns->ans_count));
    printf("\n %d个授权服务", ntohs(dns->auth_count));
    printf("\n %d个附加记录\n\n", ntohs(dns->add_count));

    /*
    **解析接收报文
    */
    reader = reader + sizeof(short);//short类型长度为32为，相当于域名字段长度，这时reader指向回答区域的查询类型字段
    answers[i].resource = (struct R_DATA*) (reader);
    reader = reader + sizeof(struct R_DATA);//指向回答问题区域的资源数据字段
    if (ntohs(answers[i].resource->type) == A) //判断资源类型是否为IPv4地址
    {
        answers[i].rdata = (unsigned char*) malloc(ntohs(answers[i].resource->data_len));//资源数据
    for (j = 0; j < ntohs(answers[i].resource->data_len); j++)
    {
        answers[i].rdata[j] = reader[j];
    }
    answers[i].rdata[ntohs(answers[i].resource->data_len)] = '\0';
    reader = reader + ntohs(answers[i].resource->data_len);
    }

    //显示查询结果
    if (ntohs(answers[i].resource->type) == A) //判断查询类型IPv4地址
    {
        long *p;
        p = (long*) answers[i].rdata;
        a.sin_addr.s_addr = *p;
        printf("IPv4地址:%s\n", inet_ntoa(a.sin_addr));
        sprintf(ipaddr,"%s",inet_ntoa(a.sin_addr));
    }

    return;
}


