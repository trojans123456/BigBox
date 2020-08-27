/* Definitions for use with Linux AF_PACKET sockets.
   Copyright (C) 1998-2016 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#ifndef __NETPACKET_PACKET_H
#define __NETPACKET_PACKET_H	1
/** 表示设备无关的物理层地址结构 */
struct sockaddr_ll
  {
    unsigned short int sll_family; //地址簇
    unsigned short int sll_protocol;//上层协议类型 if_ether.h ETH_P_XX
    int sll_ifindex; //接口类型 netdevice.h IF_PORT_XXX
    unsigned short int sll_hatype; //arp 硬件地址类型 net/if_arp.h ARPHRD_XXX
    unsigned char sll_pkttype;//包含分组类型 下面的pack_xxx
    unsigned char sll_halen; //物理层地址长度 if_ether.h
    unsigned char sll_addr[8]; //物理层地址
  };

/* Packet types.  */
//包地址为本地主机地址
#define PACKET_HOST		0		/* To us.  */
//物理层广播
#define PACKET_BROADCAST	1		/* To all.  */
//发送到物理层多播地址的包
#define PACKET_MULTICAST	2		/* To group.  */

#define PACKET_OTHERHOST	3		/* To someone else.  */
//本地回环包
#define PACKET_OUTGOING		4		/* Originated by us . */
#define PACKET_LOOPBACK		5
#define PACKET_FASTROUTE	6

/* Packet socket options.  */

#define PACKET_ADD_MEMBERSHIP		1
#define PACKET_DROP_MEMBERSHIP		2
#define	PACKET_RECV_OUTPUT		3
#define	PACKET_RX_RING			5
#define	PACKET_STATISTICS		6
#define	PACKET_COPY_THRESH		7
#define	PACKET_AUXDATA			8
#define	PACKET_ORIGDEV			9
#define	PACKET_VERSION			10
#define	PACKET_HDRLEN			11
#define	PACKET_RESERVE			12
#define	PACKET_TX_RING			13
#define	PACKET_LOSS			14
#define	PACKET_VNET_HDR			15
#define	PACKET_TX_TIMESTAMP		16
#define	PACKET_TIMESTAMP		17
#define	PACKET_FANOUT			18
#define	PACKET_TX_HAS_OFF		19
#define	PACKET_QDISC_BYPASS		20
#define	PACKET_ROLLOVER_STATS		21
#define	PACKET_FANOUT_DATA		22

//给setsocketopt 使用
struct packet_mreq
  {
    int mr_ifindex;
    unsigned short int mr_type;
    unsigned short int mr_alen;
    unsigned char mr_address[8];
  };

#define PACKET_MR_MULTICAST	0
#define PACKET_MR_PROMISC	1
#define PACKET_MR_ALLMULTI	2
#define PACKET_MR_UNICAST	3

#endif	/* netpacket/packet.h */
