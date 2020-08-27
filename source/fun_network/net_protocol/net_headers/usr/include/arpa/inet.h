/* Copyright (C) 1997-2016 Free Software Foundation, Inc.
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

#ifndef _ARPA_INET_H
#define	_ARPA_INET_H	1

#include <features.h>
#include <netinet/in.h>		/* To define `struct in_addr'.  */

/* Type for length arguments in socket calls.  */
#ifndef __socklen_t_defined
typedef __socklen_t socklen_t;
# define __socklen_t_defined
#endif

__BEGIN_DECLS

/**将点分ip转换为in_addr结构 192.168.1.1 -> 3232235777*/
extern in_addr_t inet_addr (const char *__cp) __THROW;

/** Return the local host address part of the Internet address in IN.  */
extern in_addr_t inet_lnaof (struct in_addr __in) __THROW;

/* Make Internet host address in network byte order by combining the
   network number NET with the local address HOST.  */
extern struct in_addr inet_makeaddr (in_addr_t __net, in_addr_t __host)
     __THROW;

/* Return network number part of the Internet address IN.  */
extern in_addr_t inet_netof (struct in_addr __in) __THROW;

/* Extract the network number in network byte order from the address
   in numbers-and-dots natation starting at CP.  */
/**字符串格式ip转换为整数*/
extern in_addr_t inet_network (const char *__cp) __THROW;

/*将in_addr转换为点分格式  net to addr 线程不安全，内部使用静态buffer，返回的是一个静态数组*/
extern char *inet_ntoa (struct in_addr __in) __THROW;

/*将点分格式转换为网络整数，cp是点分字符串，buf是转完后的s_addr*/
extern int inet_pton (int __af, const char *__restrict __cp,
		      void *__restrict __buf) __THROW;

/*将整数转换为点分格式 cp->buf*/
extern const char *inet_ntop (int __af, const void *__restrict __cp,
			      char *__restrict __buf, socklen_t __len)
     __THROW;


/* The following functions are not part of XNS 5.2.  */
#ifdef __USE_MISC
/**将点分格式转换为s_addr cp为点分格式*/
extern int inet_aton (const char *__cp, struct in_addr *__inp) __THROW;

/* Format a network number NET into presentation format and place result
   in buffer starting at BUF with length of LEN bytes.  */
extern char *inet_neta (in_addr_t __net, char *__buf, size_t __len) __THROW;

/* Convert network number for interface type AF in buffer starting at
   CP to presentation format.  The result will specifiy BITS bits of
   the number.  */
extern char *inet_net_ntop (int __af, const void *__cp, int __bits,
			    char *__buf, size_t __len) __THROW;

/* Convert network number for interface type AF from presentation in
   buffer starting at CP to network format and store result int
   buffer starting at BUF of size LEN.  */
extern int inet_net_pton (int __af, const char *__cp,
			  void *__buf, size_t __len) __THROW;

/* Convert ASCII representation in hexadecimal form of the Internet
   address to binary form and place result in buffer of length LEN
   starting at BUF.  */
extern unsigned int inet_nsap_addr (const char *__cp,
				    unsigned char *__buf, int __len) __THROW;

/* Convert internet address in binary form in LEN bytes starting at CP
   a presentation form and place result in BUF.  */
extern char *inet_nsap_ntoa (int __len, const unsigned char *__cp,
			     char *__buf) __THROW;
#endif

__END_DECLS

#endif /* arpa/inet.h */
