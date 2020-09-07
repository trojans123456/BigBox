#include <stdio.h>
#ifndef __WIN32__
#include <sys/sysinfo.h>
#endif
#include "free.h"

/**
 * struct sysinfo
 * {
 * long uptime; //系统启动到现在的时间
 * unsigned long loads[3];		//
 * unsigned long totalram;		// 总的可用的内存大小
 * unsigned long freeram;		//还未被使用的内存大小
 * unsigned long sharedram;	//共享的存储器的大小
 * unsigned long bufferram;	//缓冲区大小
 * unsigned long totalswap;	//交换区大小
 * unsigned long freeswap;		//还可用的交换区大小
 * unsigned short procs;		//当前进程数目
 * unsigned long totalhigh;	//总的高内存大小
 * unsigned long freehigh;		//可用的高内存大小
 * unsigned int mem_unit;		//以字节为单位的内存大小
 * char _f[20 - 2*sizeof(long) - sizeof(int)]; //补丁
 * };
 *
 *
 *
**/

int sysinfo_free()
{
    struct sysinfo info;
    int ret = sysinfo(&info);
    if(ret < 0)
    {
        return ret;
    }
    printf("mem_uint = %d \n",info.mem_unit);
    /* 兼容2.4 */
    if(info.mem_unit == 0)
        info.mem_unit = 1;

    if(info.mem_unit == 1)
    {
        info.mem_unit = 1024;

        info.totalram /= info.mem_unit;
        info.freeram /= info.mem_unit;

        info.totalswap /= info.mem_unit;
        info.freeswap /= info.mem_unit;

        info.sharedram /= info.mem_unit;
        info.bufferram /= info.mem_unit;
    }
    else
    {
        info.mem_unit = 1024;
        info.totalram *= info.mem_unit;
        info.freeram *= info.mem_unit;

        info.totalswap *= info.mem_unit;
        info.freeswap *= info.mem_unit;

        info.sharedram *= info.mem_unit;
        info.bufferram *= info.mem_unit;
    }

    printf("%6s%13s%13s%13s%13s%13s\n", "", "total", "used", "free",
           "shared", "buffers");

    printf("%6s%13ld%13ld%13ld%13ld%13ld\n", "Mem:", info.totalram,
           info.totalram-info.freeram, info.freeram,
           info.sharedram, info.bufferram);


    printf("%6s%13ld%13ld%13ld\n", "Swap:", info.totalswap,
           info.totalswap-info.freeswap, info.freeswap);

    printf("%6s%13ld%13ld%13ld\n", "Total:", info.totalram+info.totalswap,
           (info.totalram-info.freeram)+(info.totalswap-info.freeswap),
           info.freeram+info.freeswap);

    return 1;
}
