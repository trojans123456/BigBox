#include "dhcp.h"


#include <sys/ioctl.h>
#include <string.h>
#include <stdlib.h>

#include <net/if.h>

unsigned short csum_ip(unsigned short *buf, int nwords)
{
	//Checksum the buffer
    unsigned long sum;
    for(sum=0; nwords>0; nwords--)
        sum += *buf++;
    sum = (sum >> 16) + (sum &0xffff);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}

struct hw_eth_iface find_iface(int sock_fd, char* iface_name)
{
    struct hw_eth_iface iface;
    struct ifreq ifr;
    strncpy(ifr.ifr_name, iface_name, IFNAMSIZ);

	//Get the iface index
    ioctl(sock_fd, SIOCGIFINDEX, &ifr);
    iface.index = ifr.ifr_ifindex;

	//Get the iface HW address
    ioctl(sock_fd, SIOCGIFHWADDR, &ifr);

	//Copy the address in our structure
//    memcpy(iface.hw_addr, ifr.ifr_hwaddr.sa_data, 6); // LINK problem
	int i;
	for(i = 0; i < 6; i++)
		iface.hw_addr[i] = ifr.ifr_hwaddr.sa_data[i];
    iface.addr_len = 6;

    return iface;
}

int build_upd_hdr(void* ptr, unsigned short data_len, unsigned short src_port, unsigned short dst_port)
{
    struct udpheader *udp = (struct udpheader*)ptr;
    int len = sizeof(struct udpheader);
    udp->udph_srcport = htons(68);
    udp->udph_destport = htons(67);
    udp->udph_len = htons(len + data_len);
    //TODO : UDP checksum
    return len;
}

int build_ip4_hdr(void *ptr, unsigned short data_len, char* src_addr, char* dst_addr, unsigned char proto)
{
    struct ipheader *ip = (struct ipheader*)ptr;
    int len = sizeof(struct ipheader);
    ip->iph_ihl = 5;
    ip->iph_ver = 4;
    ip->iph_tos = 0;
    ip->iph_offset = 0;
    ip->iph_len = htons(len + data_len);
    ip->iph_ident = htons(54321); //TODO: random number
    ip->iph_ttl = 64; // hops
    ip->iph_protocol = proto;
    ip->iph_sourceip = inet_addr(src_addr);
    ip->iph_destip = inet_addr(dst_addr);
    ip->iph_chksum = csum_ip((unsigned short *)ip, len/2);

    return len;
}

int build_ip4_udp_pkt(unsigned char* buffer, int buff_len, unsigned char* data, unsigned short data_len, char* src_addr, char* dst_addr, unsigned short src_port, unsigned short dst_port, unsigned char proto)
{
    memset(buffer, 0, buff_len);

    struct ipheader *ip   = (struct ipheader*) buffer;
    struct udpheader *udp = (struct udpheader*)(buffer + sizeof(struct ipheader));

    int udp_len  = build_upd_hdr(udp, data_len, src_port, dst_port);
    int ip_len   = build_ip4_hdr(ip, data_len + udp_len, src_addr, dst_addr, proto);
//    memcpy(buffer + sizeof(struct udpheader) + sizeof(struct ipheader), data, data_len); // LINK problem
	int i;
	int offset = sizeof(struct udpheader) + sizeof(struct ipheader);
	for(i = 0; i < data_len; i++)
		buffer[offset + i] = data[i];

    return data_len + udp_len + ip_len;
}

/************** dhcp *****************/

int build_dhcp_discover(struct dhcp_pkt* pkt, unsigned char* src_mac, int mac_len)
{
    memset(pkt, 0, sizeof(struct dhcp_pkt));
    pkt->op      = OP_BOOT_REQUEST;
    pkt->htype   = HW_TYPE_ETHERNET;
    pkt->hlen    = HW_LENGTH_ETHERNET;
    pkt->hops    = 0x00;
    pkt->xid     = 0x3903f326; //TODO: Random transaction ID
    pkt->secs    = 0x0000;
    pkt->flags   = 0x0000;
    pkt->ci_addr = 0x00000000;
    pkt->yi_addr = 0x00000000;
    pkt->si_addr = 0x00000000;
    pkt->gi_addr = 0x00000000;

    // memcpy(pkt->cm_addr, src_mac, mac_len); // LINK problem
	int i;
	for(i = 0; i < mac_len; i++)
		pkt->cm_addr[i] = src_mac[i];

    pkt->magic = DHCP_MAGIC;

    //Add DHCP options
    pkt->opt[0] = OPTION_DHCP_MESSAGE_TYPE;
    pkt->opt[1] = 0x01;
    pkt->opt[2] = VALUE_MESSAGE_DISCOVER;

    pkt->opt[3] = OPTION_PARAMETER_REQUEST_LIST;
    pkt->opt[4] = 0x03;
    pkt->opt[5] = OPTION_ROUTER; // Ask for gateway address
    pkt->opt[6] = OPTION_SUBNET_MASK; // Ask for the netmask
    pkt->opt[7] = OPTION_DNS; // ASK for DNS address

    pkt->opt[8] = DHCP_END;


    //TODO : Use the same procedure to write options, than the one used to read options
//    pkt->opt[0].id = 53;
//    pkt->opt[0].len = 0x01;
//    pkt->opt[0].values[0] = 0x01;
//
//    pkt->opt[1].id = DHCP_END;

    return sizeof(struct dhcp_pkt);
}

int is_dhcp(struct dhcp_pkt* pkt)
{
	// It's a DHCP packet if dhcp magic number is good
	//TODO: check the packet length ?
    return pkt->magic == DHCP_MAGIC;
}

struct dhcp_opt* get_dhcp_option(struct dhcp_pkt *pkt, int *offset)
{
    if(pkt->opt[*offset] == 0x00 || pkt->opt[*offset] == DHCP_END)
        return NULL;
	// If the opt != end or != empty, cast the memory zone into a option struct, and return it
	struct dhcp_opt* opt = (struct dhcp_opt*)&(pkt->opt[*offset]);
	*offset += sizeof(struct dhcp_opt) + opt->len;
    return opt;
}

unsigned int char_to_ip(unsigned char* ip)
{
	return htonl(ip[0] << 24 | ip[1] << 16 | ip[2] << 8 | ip[3]);
}
