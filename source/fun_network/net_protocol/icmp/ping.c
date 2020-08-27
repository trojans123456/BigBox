#include <stdio.h>
#include "icmp.h"

int main(int argc,char *argv[])
{
    net_ping("www.baidu.com",80,10,2);

    return 0;
}
