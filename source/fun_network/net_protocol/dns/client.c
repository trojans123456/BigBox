#include <stdio.h>

#include "dns.h"

int main(int argc,char *argv[])
{
	unsigned char buf[128] = "";

	unsigned char dns[128] = "unicronlogic-test.xiaojukeji.com";

	dns_parser(dns,buf);

	printf("%s \n",buf);
}
