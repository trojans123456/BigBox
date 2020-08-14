#include <stdio.h>

#include "utime.h"

int main(int argc,char *argv[])
{
    uint64_t tm = time_sec();
    printf("tm = %ld\n",tm);

    printf("tm = %ld\n",time_nsec_bootup());

    struct sys_time sys_tm;

    int ret = get_systime(&sys_tm);
    if(ret == 0)
    {
        char buffer[128];
        time_str_format_by_struct(&sys_tm,buffer,128);
        printf("%s \n",buffer);
    }
    return 0;
}
