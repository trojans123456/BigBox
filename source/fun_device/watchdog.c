#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/watchdog.h>
#include <fcntl.h>

#include "watchdog.h"


int wdt_start(const char *wdt_name)
{
    if(NULL == wdt_name)
        return -1;

    int fd = open(wdt_name,O_RDWR);
    if(fd < 0)
        return -1;

    return fd;
}

void wdt_stop(int fd)
{
    if(fd > 0)
        close(fd);
}

int wdt_get_timeout(int fd, int *timeout)
{
    if((fd < 0) || !timeout)
        return -1;

    int tm;
    int ret = ioctl(fd,WDIOC_GETTIMEOUT,&tm);
    if(ret < 0)
        return -1;

    *timeout = tm;

    return 0;
}

int wdt_set_timeout(int fd,int timeout)
{
    if(fd < 0)
        return -1;

    int ret = ioctl(fd,WDIOC_SETTIMEOUT,&timeout);
    if(ret < 0)
        return -1;

    return 0;
}

int wdt_keep_alive(int fd)
{
    if(fd < 0)
        return -1;

    int ret = write(fd,"\0",1);
    if(ret < 0)
        return -1;

    return 0;
}
