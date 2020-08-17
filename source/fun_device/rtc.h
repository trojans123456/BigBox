#ifndef __RTC_H_
#define __RTC_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    unsigned short year;//1-9999
    unsigned char month;//1-12
    unsigned char day;//1-31
    unsigned char hour;//0-23
    unsigned char minute;//0-59
    unsigned char second;//0-59
}datetime_t;

int rtc_open(const char *devname);
void rtc_close(int fd);

int rtc_read_time(int fd,datetime_t *tm);
int rtc_set_time(int fd,datetime_t *tm);
int rtc_read_alarm(int fd,datetime_t *tm);
int rtc_set_alarm(int fd,datetime_t *tm);
int rtc_enable_alarm(int fd,int enabled);


#ifdef __cplusplus
}
#endif

#endif // E_RTC_H_
