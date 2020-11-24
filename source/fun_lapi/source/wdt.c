#include <stdio.h>
#include <string.h>

#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#ifdef __linux
#include <sys/ioctl.h>
#include <linux/watchdog.h>
#endif
#include <fcntl.h>
#include "wdt.h"

typedef struct
{
    int fd;
}wdt_t;

hWDT wdt_start(const char *wdt_name)
{
    wdt_t *wdt = (wdt_t *)calloc(1,sizeof(wdt_t));
    if(!wdt)
        return NULL;

    if(NULL == wdt_name)
     {
        free(wdt);
        return NULL;
    }

    wdt->fd = open(wdt_name,O_RDWR);
    if(wdt->fd < 0)
    {
        free(wdt);
        return NULL;
    }

    return (hWDT)wdt;
}

void wdt_stop(hWDT w)
{
    wdt_t *wdt = (wdt_t *)w;
    if(!wdt)
        return ;

    if(wdt->fd > 0)
        close(wdt->fd);
    free(wdt);
}

int wdt_get_timeout(hWDT w, int *timeout)
{
    wdt_t *wdt = (wdt_t*)w;
    if(!wdt)
        return -1;

    if((wdt->fd < 0) || !timeout)
        return -1;

    int tm;
    int ret = ioctl(wdt->fd,WDIOC_GETTIMEOUT,&tm);
    if(ret < 0)
        return -1;

    *timeout = tm;

    return 0;
}

int wdt_set_timeout(hWDT w,int timeout)
{
    wdt_t *wdt = (wdt_t*)w;
    if(!wdt)
        return -1;

    if(wdt->fd < 0)
        return -1;

    int ret = ioctl(wdt->fd,WDIOC_SETTIMEOUT,&timeout);
    if(ret < 0)
        return -1;

    return 0;
}

int wdt_keep_alive(hWDT w)
{
    wdt_t *wdt = (wdt_t *)w;
    if(!wdt)
        return -1;

    if(wdt->fd < 0)
        return -1;

    int ret = write(wdt->fd,"\0",1);
    if(ret < 0)
        return -1;

    return 0;
}
