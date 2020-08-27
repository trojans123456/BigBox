#ifndef DNS_H
#define DNS_H

/*
**DNS�����в�ѯ����Ĳ�ѯ����
*/
#define A 1 //��ѯ���ͣ���ʾ���������IPv4��ַ


/*
**DNS�����ײ�
**����ʹ����λ��
*/
struct DNS_HEADER {
    unsigned short id; //�Ự��ʶ
    unsigned char rd :1; // ��ʾ�����ݹ�
    unsigned char tc :1; // ��ʾ�ɽضϵ�
    unsigned char aa :1; //  ��ʾ��Ȩ�ش�
    unsigned char opcode :4;
    unsigned char qr :1; //  ��ѯ/��Ӧ��־��0Ϊ��ѯ��1Ϊ��Ӧ
    unsigned char rcode :4; //Ӧ����
    unsigned char cd :1;
    unsigned char ad :1;
    unsigned char z :1; //����ֵ
    unsigned char ra :1; // ��ʾ���õݹ�
    unsigned short q_count; // ��ʾ��ѯ��������ڵ�����
    unsigned short ans_count; // ��ʾ�ش����������
    unsigned short auth_count; // ��ʾ��Ȩ���������
    unsigned short add_count; // ��ʾ�������������
};

/*
**DNS�����в�ѯ��������
*/
struct QUESTION {
    unsigned short qtype;//��ѯ����
    unsigned short qclass;//��ѯ��
};
typedef struct {
    unsigned char *name;
    struct QUESTION *ques;
} QUERY;

/*
**DNS�����лش�����ĳ����ֶ�
*/
//�����Ƶ�����
#pragma pack(push, 1)//�������״̬���趨Ϊ1�ֽڶ���
struct R_DATA {
    unsigned short type; //��ʾ��Դ��¼������
    unsigned short _class; //��
    unsigned int ttl; //��ʾ��Դ��¼���Ի����ʱ��
    unsigned short data_len; //���ݳ���
};
#pragma pack(pop) //�ָ�����״̬

/*
**DNS�����лش��������Դ�����ֶ�
*/
struct RES_RECORD {
    unsigned char *name;//��Դ��¼����������
    struct R_DATA *resource;//��Դ����
    unsigned char *rdata;
};

#define DNS_SERVER_IPADDR   "8.8.8.8"
void dns_parser(unsigned char *host, unsigned char *ipaddr);

#endif
