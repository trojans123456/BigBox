#ifndef __ASM_GENERIC_SOCKET_H
#define __ASM_GENERIC_SOCKET_H

#include <asm/sockios.h>

/** Ϊsetsockopt����׼�� */
/* For setsockopt(2) */
#define SOL_SOCKET	1

/**** SOL_SOCKET ** level*/

/** ������� */
#define SO_DEBUG	1
/** �ö˿��ͷź������Ϳ��Ա��ٴ�ʹ�� (Ĭ��2���Ӻ���ܱ�ʹ��)*/
#define SO_REUSEADDR	2
//��ȡ�׽�������
#define SO_TYPE		3
//��ȡ�׽��ִ���
#define SO_ERROR	4
/** ������·�� */
#define SO_DONTROUTE	5
/** �����͹㲥���� */
#define SO_BROADCAST	6
/**���ͻ�������С */
#define SO_SNDBUF	7
//���ջ�������С
#define SO_RCVBUF	8
#define SO_SNDBUFFORCE	32
#define SO_RCVBUFFORCE	33
//��������
#define SO_KEEPALIVE	9
//�������ݷ�������������
#define SO_OOBINLINE	10
#define SO_NO_CHECK	11
#define SO_PRIORITY	12
#define SO_LINGER	13
//��bsdϵͳ����
#define SO_BSDCOMPAT	14
#define SO_REUSEPORT	15
#ifndef SO_PASSCRED /* powerpc only differs in these */
#define SO_PASSCRED	16
#define SO_PEERCRED	17
//���ջ���������
#define SO_RCVLOWAT	18
//���ͻ���������
#define SO_SNDLOWAT	19
//���ճ�ʱ
#define SO_RCVTIMEO	20
//���ͳ�ʱ
#define SO_SNDTIMEO	21
#endif

/* Security levels - as per NRL IPv6 - don't actually do anything */
#define SO_SECURITY_AUTHENTICATION		22
#define SO_SECURITY_ENCRYPTION_TRANSPORT	23
#define SO_SECURITY_ENCRYPTION_NETWORK		24

#define SO_BINDTODEVICE	25

/* Socket filtering */
#define SO_ATTACH_FILTER	26
#define SO_DETACH_FILTER	27
#define SO_GET_FILTER		SO_ATTACH_FILTER

#define SO_PEERNAME		28
#define SO_TIMESTAMP		29
#define SCM_TIMESTAMP		SO_TIMESTAMP

#define SO_ACCEPTCONN		30

#define SO_PEERSEC		31
#define SO_PASSSEC		34
#define SO_TIMESTAMPNS		35
#define SCM_TIMESTAMPNS		SO_TIMESTAMPNS

#define SO_MARK			36

#define SO_TIMESTAMPING		37
#define SCM_TIMESTAMPING	SO_TIMESTAMPING

#define SO_PROTOCOL		38
#define SO_DOMAIN		39

#define SO_RXQ_OVFL             40

#define SO_WIFI_STATUS		41
#define SCM_WIFI_STATUS	SO_WIFI_STATUS
#define SO_PEEK_OFF		42

/* Instruct lower device to use last 4-bytes of skb data as FCS */
#define SO_NOFCS		43

#define SO_LOCK_FILTER		44

#define SO_SELECT_ERR_QUEUE	45

#define SO_BUSY_POLL		46

#define SO_MAX_PACING_RATE	47

#define SO_BPF_EXTENSIONS	48

#define SO_INCOMING_CPU		49

#define SO_ATTACH_BPF		50
#define SO_DETACH_BPF		SO_DETACH_FILTER

#endif /* __ASM_GENERIC_SOCKET_H */
