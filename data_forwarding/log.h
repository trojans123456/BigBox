#ifndef __LOG_H
#define __LOG_H

#ifdef __cplusplus
extern "C" {
#endif

#define LOG_LEVEL_DEBUG     0
#define LOG_LEVEL_INFO      1
#define LOG_LEVEL_ERR       2

#ifdef __GNUC__
/**
a：第几个参数为格式化字符串(format string)
b：参数集合中的第一个，即参数“…”里的第一个参数在函数参数总数排在第几
*/
#define EV_CHECK_FMT(a,b)   __attribute__((format(printf, a, b)))
#else
#define EV_CHECK_FMT(a,b)
#endif

typedef void (*log_output)(int level,const char *msg);
void set_output_fn(log_output ptr);

void log_err(int val,const char *fmt,...)   EV_CHECK_FMT(2,3);
void log_info(const char *fmt,...)          EV_CHECK_FMT(1,2);
void log_debug(const char *fmt,...)         EV_CHECK_FMT(1,2);

#if defined(DEBUG_USED)
#define debug(x,...)    log_debug(x,__VA_ARGS__);
#elif
#define debug(x,...)
#endif


#ifdef __cplusplus
}
#endif

#endif
