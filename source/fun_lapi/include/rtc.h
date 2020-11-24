#ifndef __RTC_H
#define __RTC_H

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

declear_handler(hRTC);

typedef struct
{
    unsigned short year;//1-9999
    unsigned char month;//1-12
    unsigned char day;//1-31
    unsigned char hour;//0-23
    unsigned char minute;//0-59
    unsigned char second;//0-59
}datetime_t;

hRTC rtc_open(const char *devname);
void rtc_close(hRTC r);

int rtc_read_time(hRTC r,datetime_t *tm);
int rtc_set_time(hRTC r,datetime_t *tm);
int rtc_read_alarm(hRTC r,datetime_t *tm);
int rtc_set_alarm(hRTC r,datetime_t *tm);
int rtc_enable_alarm(hRTC r,int enabled);


#ifdef __cplusplus
}
#endif

#endif // E_RTC_H_
