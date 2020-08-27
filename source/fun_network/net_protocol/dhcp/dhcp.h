#ifndef DHCP_H
#define DHCP_H


/**ip头
*/
struct ipheader
{
    unsigned int    iph_ihl:4; 	//IP header length in 32 bits words
    unsigned int    iph_ver:4; 	//IP version
    unsigned char   iph_tos; 	// Type of service
    unsigned short  iph_len; 	// Total packet length
    unsigned short  iph_ident; 	// Identification
    unsigned short  iph_offset; //Fragment offset
    unsigned char   iph_ttl; 	// Time to live
    unsigned char   iph_protocol; // Protocol
    unsigned short  iph_chksum;	  // Header checksum
    unsigned int    iph_sourceip; // Sender IP address
    unsigned int    iph_destip;   // Destination IP address
};

/** udp 头
*/
struct udpheader
{
    unsigned short udph_srcport;    // Scource port
    unsigned short udph_destport;   // Destination port
    unsigned short udph_len;        // Length (UDP + data)
    unsigned short udph_chksum;     // UDP checksum
};

/**ethernet interface
*/
struct hw_eth_iface
{
    int  index;         // Index identifying the HW interface in the system
    int  addr_len : 6;  // HW address lentgh
    char hw_addr[6];    // HW address
};

/** 校验和计算功能
*/
unsigned short csum_ip(unsigned short *buf, int nwords);

/** 根据其名称查找HW网络接口
*/
struct hw_eth_iface find_iface(int sock_fd, char* iface_name);

/** 创建一个udp头
*/
int build_upd_hdr(void* ptr, unsigned short len, unsigned short src_port, unsigned short dst_port);

/** 创建一个ipv4 头
*/
int build_ip4_hdr(void *ptr, unsigned short data_len, char* src_addr, char* dst_addr, unsigned char proto);

/** 在ipv4的基础上创建udp头
*/
int build_ip4_udp_pkt(unsigned char* buffer, int buff_len, unsigned char* data, unsigned short data_len, char* src_addr, char* dst_addr, unsigned short src_port, unsigned short dst_port, unsigned char proto);

/***************** dhcp **********************/
#define DHCP_MAGIC 			htonl(0x63825363)

#define DHCP_MIN_PACK_SIZE 	240

#define OP_BOOT_REQUEST 	0x01
#define OP_BOOT_REPLY 		0x02

#define HW_TYPE_ETHERNET 	0x01
#define HW_LENGTH_ETHERNET 	0x06

//DHCP options

#define DHCP_END 	0xff

#define OPTION_DHCP_MESSAGE_TYPE 	53
/** dhcp 报文类型 */
#define VALUE_MESSAGE_DISCOVER 		0x01
#define VALUE_MESSAGE_OFFER 		0x02
#define VALUE_MESSAGE_REQUEST 		0x01
#define VALUE_MESSAGE_ACK 			0x05
#define VALUE_MESSAGE_NAK 			0x06
#define VALUE_MESSAGE_INFORM 		0x08

#define OPTION_SERVER_IP 			54
#define OPTION_LEASE_TIME 			51
#define OPTION_REQUESTED_IP 		50

#define OPTION_PARAMETER_REQUEST_LIST 	55

#define OPTION_SUBNET_MASK 		1
#define OPTION_ROUTER 			3
#define OPTION_BROADCAST_ADDR 	28
#define OPTION_DNS 				6
#define OPTION_DOMAIN_NAME 		15
#define OPTION_HOST_NAME 		12

//These should not really be usefull for what we do
#define OPTION_TIME_OFFSET 		2
#define OPTION_STATIC_ROUTE 	121
#define OPTION_NIS_DOMAIN 		40
#define OPTION_NIS_SERVERS 		41
#define OPTION_NTP_SERVERS 		42
#define OPTION_MTU 				26
#define OPTION_DOMAIN_SEARCH 	119

//DHCP options
struct dhcp_opt {
    unsigned char id;       // Option ID
    unsigned char len;      // Option value length
    //unsigned char values[];
    // Option value(s)  和 values[0] 一样不占用字节,有些编译器可能不支持。所以提供一个宏 ip_addr替换
};
#define IP_ADDR(opt)    (opt + sizeof(struct dhcp_opt))

/** dhcp 报文*/
struct dhcp_pkt
{
    unsigned char   op;     // Message type
    unsigned char   htype;  // HW type
    unsigned char   hlen;   // HW addr length
    unsigned char   hops;   // Hops

    unsigned int    xid;    // Transaction ID

    unsigned short  secs;   // seconds elapsed
    unsigned short  flags;  // Bootp flags

    unsigned int    ci_addr; // Client address
    unsigned int    yi_addr; // Your address
    unsigned int    si_addr; // Next Server IP address
    unsigned int    gi_addr; // Relay agent IP address
    unsigned char   cm_addr[6];   // Client MAC address
    unsigned char   ch_addr[10];  // Client hardware address padding

    unsigned char   unused[192];

    unsigned int    magic;      // DHCP magic number

    unsigned char   opt[128];   // Options padding
//    struct dhcp_opt opt [64];
};

/**建立一个发现DHCP数据包，返回数据包大小 */
int build_dhcp_discover(struct dhcp_pkt* pkt, unsigned char* src_mac, int mac_len);

/** 检查数据包是否是DHCP */
int is_dhcp(struct dhcp_pkt* pkt);

/** 在DHCP包中找到一个选项。偏移量是选项在包中的位置。
调用后，偏移量被更新为下一个选项的偏移量(如果有的话)*/
struct dhcp_opt* get_dhcp_option(struct dhcp_pkt *pkt, int *offset);

/** 读取4字节数据数组中的IP */
unsigned int char_to_ip(unsigned char* ip);


#endif // DHCP_H
