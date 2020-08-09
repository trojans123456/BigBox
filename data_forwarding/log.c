#include <stdarg.h>
#include <stdio.h>

#include "log.h"

static log_output _output = NULL;

void set_output_fn(log_output ptr)
{
    _output = ptr;
}

static void helper(int level,const char *fmt,va_list ap)
{
    char buf[1024] = "";
    int len = 0;

    if(fmt != NULL)
    {
        int r = vsnprintf(buf,sizeof(buf),fmt,ap);
        buf[r] = '\0';
    }
    else
    {
        buf[0] = '\0';
    }

    if(_output)
    {
        _output(level,buf);
        return ;
    }

    const char *prefix = NULL;
    switch(level)
    {
    case LOG_LEVEL_DEBUG:
        prefix = "[debug]";
        break;
    case LOG_LEVEL_ERR:
        prefix = "[error]";
        break;
    case LOG_LEVEL_INFO:
        prefix = "[info]";
        break;
    default:
        prefix = "[???]";
        break;
    }

    fprintf(stdout,"[%s] %s\n",prefix,buf);
}

void log_err(int val, const char *fmt,...)
{
    va_list ap;

    va_start(ap,fmt);
    helper(LOG_LEVEL_ERR,fmt,ap);
    va_end(ap);

    exit(val);
}

void log_info(const char *fmt,...)
{
    va_list ap;
    va_start(ap,fmt);
    helper(LOG_LEVEL_INFO,fmt,ap);
    va_end(ap);
}

void log_debug(const char *fmt,...)
{
    va_list ap;
    va_star(ap,fmt);
    helper(LOG_LEVEL_DEBUG,fmt,ap);
    va_end(ap);
}
