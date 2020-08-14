#ifndef __U_TIME_H
#define __U_TIME_H

#include <stdint.h>
#include <stdbool.h>

#ifndef __WIN32__
#include <time.h>
#include <sys/time.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 获取从1970年到现在的秒数
 * @return
 */
uint64_t getCurrentTime();

/**
 * @brief 当前的秒数
 * @return
 */
uint64_t time_sec();

/**
 * @brief 字符串格式的时间
 * @return
 */
char *time_sec_str();

/**
 * @brief 将当前秒数格式化字符串形式
 * @param str
 * @param len
 * @return
 */
char *time_str_format(char *str,int len);

/**
 * @brief 将utc时间格式化为字符串形式
 * @param utc
 * @param str
 * @param len
 * @return
 */
char *time_str_format_by_utc(uint32_t utc,char *str,int len);

/**
 * @brief
 * @param msec
 * @param str
 * @param len
 * @return
 */
char *time_str_format_by_msec(uint64_t msec,char *str,int len);

/**
 * @brief
 * @param val
 * @param str
 * @param len
 * @return
 */
char *time_str_format_by_timeval(struct timeval *tv, char *str, int len);

struct sys_time
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t sec;
};

int set_systime(struct sys_time *tm);
int get_systime(struct sys_time *tm);

char *time_str_format_by_struct(struct sys_time *tm,char *str,int len);

/**
 * @brief 转换为bcd格式
 * @param tm
 * @param bcd_time char bcd[8]
 */
void time2bcd(struct sys_time *tm, char *bcd_time);
void bcd2time(struct sys_time *tm,char *bcd_time);

uint64_t time_msec();

char *time_msec_str(char *str,int len);

/**
 * @brief time_sleep_ms
 * @param ms
 * @return
 *
 * Action!!
 * 慎用，内部是select,每次都会产生int80 或 arm 系统调用中断
 */
int time_sleep_ms(uint64_t ms);

uint64_t time_usec(struct timeval *val);

/* nano second */
uint64_t time_nsec();
/**
 * @brief 系统启动到现在的时间
 * @return
 */
uint64_t time_nsec_bootup();

char *time_nsec_to_str(uint64_t nsec);

bool time_passed_sec(int sec);

#ifdef __cplusplus
}
#endif

#endif
