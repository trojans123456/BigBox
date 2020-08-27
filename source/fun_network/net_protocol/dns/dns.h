#ifndef DNS_H
#define DNS_H

/*
**DNS报文中查询区域的查询类型
*/
#define A 1 //查询类型，表示由域名获得IPv4地址


/*
**DNS报文首部
**这里使用了位域
*/
struct DNS_HEADER {
    unsigned short id; //会话标识
    unsigned char rd :1; // 表示期望递归
    unsigned char tc :1; // 表示可截断的
    unsigned char aa :1; //  表示授权回答
    unsigned char opcode :4;
    unsigned char qr :1; //  查询/响应标志，0为查询，1为响应
    unsigned char rcode :4; //应答码
    unsigned char cd :1;
    unsigned char ad :1;
    unsigned char z :1; //保留值
    unsigned char ra :1; // 表示可用递归
    unsigned short q_count; // 表示查询问题区域节的数量
    unsigned short ans_count; // 表示回答区域的数量
    unsigned short auth_count; // 表示授权区域的数量
    unsigned short add_count; // 表示附加区域的数量
};

/*
**DNS报文中查询问题区域
*/
struct QUESTION {
    unsigned short qtype;//查询类型
    unsigned short qclass;//查询类
};
typedef struct {
    unsigned char *name;
    struct QUESTION *ques;
} QUERY;

/*
**DNS报文中回答区域的常量字段
*/
//编译制导命令
#pragma pack(push, 1)//保存对齐状态，设定为1字节对齐
struct R_DATA {
    unsigned short type; //表示资源记录的类型
    unsigned short _class; //类
    unsigned int ttl; //表示资源记录可以缓存的时间
    unsigned short data_len; //数据长度
};
#pragma pack(pop) //恢复对齐状态

/*
**DNS报文中回答区域的资源数据字段
*/
struct RES_RECORD {
    unsigned char *name;//资源记录包含的域名
    struct R_DATA *resource;//资源数据
    unsigned char *rdata;
};

#define DNS_SERVER_IPADDR   "8.8.8.8"
void dns_parser(unsigned char *host, unsigned char *ipaddr);

#endif
