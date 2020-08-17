#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "rtc.h"

int rtc_open(const char *dev_name)
{
    if(NULL == dev_name)
        return -1;
    /* open rtc device */
    int fd = open(dev_name, O_RDWR);
    if (fd ==  -1)
    {
        return -1;
    }
    return fd;
}

void rtc_close(int fd)
{
    if(fd > 0)
        close(fd);
}

int rtc_read_time(int fd,datetime_t *tm)
{
    if((fd < 0) || !tm)
        return -1;

    int retval;
    struct rtc_time rtc_tm;
    /*read the RTC time/date*/
    retval = ioctl(fd, RTC_RD_TIME, &rtc_tm);
    if (retval == -1)
    {
        return -1;
    }

    tm->day = rtc_tm.tm_mday;
    tm->month = rtc_tm.tm_mon + 1;
    tm->year= rtc_tm.tm_year + 1900;
    tm->hour = rtc_tm.tm_hour;
    tm->minute = rtc_tm.tm_min;
    tm->second = rtc_tm.tm_sec;

    return 0;
}

int rtc_set_time(int fd,datetime_t *tm)
{
    if((fd < 0) || !tm)
        return -1;

    int retval;
    struct rtc_time rtc_tm;

    rtc_tm.tm_mday = tm->day ;
    rtc_tm.tm_mon = tm->month-1;
    rtc_tm.tm_year = tm->year-1900;
    rtc_tm.tm_hour = tm->hour;
    rtc_tm.tm_min = tm->minute;
    rtc_tm.tm_sec = tm->second;
    rtc_tm.tm_wday = rtc_tm.tm_yday = rtc_tm.tm_isdst = 0;

    retval = ioctl(fd, RTC_SET_TIME, &rtc_tm);
    if (retval == -1)
    {
        return -1;
    }

    return 0;
}
/*读取下一次定时中断的时间*/
int rtc_read_alarm(int fd,datetime_t *tm)
{
    if((fd < 0) || !tm)
        return -1;

    int retval;
    struct rtc_time rtc_tm;
    /*read the RTC time/date*/
    retval = ioctl(fd, RTC_ALM_READ, &rtc_tm);
    if (retval == -1)
    {
        return -1;
    }

    tm->day = rtc_tm.tm_mday;
    tm->month = rtc_tm.tm_mon + 1;
    tm->year= rtc_tm.tm_year + 1900;
    tm->hour = rtc_tm.tm_hour;
    tm->minute = rtc_tm.tm_min;
    tm->second = rtc_tm.tm_sec;

    return 0;
}
//设置下一次定时中断的时间
int rtc_set_alarm(int fd,datetime_t *tm)
{
    if((fd < 0) || !tm)
        return -1;

    int retval;
    struct rtc_time rtc_tm;

    rtc_tm.tm_mday = tm->day ;
    rtc_tm.tm_mon = tm->month-1;
    rtc_tm.tm_year = tm->year-1900;
    rtc_tm.tm_hour = tm->hour;
    rtc_tm.tm_min = tm->minute;
    rtc_tm.tm_sec = tm->second;
    rtc_tm.tm_wday = rtc_tm.tm_yday = rtc_tm.tm_isdst = 0;

    retval = ioctl(fd, RTC_ALM_SET, &rtc_tm);
    if (retval == -1)
    {
        return -1;
    }

    return 0;
}

int rtc_enable_alarm(int fd,int enabled)
{
    if(fd < 0)
        return -1;

    int retval;
    int rtc_enabled = enabled ? RTC_AIE_ON : RTC_AIE_OFF;
    retval = ioctl(fd, rtc_enabled, NULL);
    if (retval == -1)
    {
        return -1;
    }

    return 0;
}
