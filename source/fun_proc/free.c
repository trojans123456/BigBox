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

float get_cpuUsage()
{
    FILE *fp;
    char buf[128];
    char cpu[5];
    long int user,nice,sys,idle,iowait,irq,softirq;

    long int all1,all2,idle1,idle2;
    float usage;

    char result[32] = "";
    fp = fopen("/proc/stat","r");
    if(fp == NULL)
    {
        return "";
    }


    fgets(buf,sizeof(buf),fp);
    sscanf(buf,"%s%ld%ld%ld%ld%ld%ld%ld",cpu,&user,&nice,&sys,&idle,&iowait,&irq,&softirq);

    all1 = user+nice+sys+idle+iowait+irq+softirq;
    idle1 = idle;
    rewind(fp);
    /*第二次取数据*/
    sleep(1);
    memset(buf,0,sizeof(buf));
    cpu[0] = '\0';
    user=nice=sys=idle=iowait=irq=softirq=0;
    fgets(buf,sizeof(buf),fp);
    //printf("buf=%s",buf);
    sscanf(buf,"%s%ld%ld%ld%ld%ld%ld%ld",cpu,&user,&nice,&sys,&idle,&iowait,&irq,&softirq);

    all2 = user+nice+sys+idle+iowait+irq+softirq;
    idle2 = idle;

    usage = (float)(all2-all1-(idle2-idle1)) / (all2-all1)*100 ;


    //sprintf(result,"%.2f%%",usage);
    fclose(fp);

    return result;
}

struct Total_Cpu_Occupy_t{
    unsigned long user;
    unsigned long nice;
    unsigned long system;
    unsigned long idle;
};


struct Proc_Cpu_Occupy_t{
    unsigned int pid;
    unsigned long utime;  //user time
    unsigned long stime;  //kernel time
    unsigned long cutime; //all user time
    unsigned long cstime; //all dead time
};

unsigned long get_cpu_total_occupy()
{
    FILE *fd;
    char buff[1024]={0};
    struct Total_Cpu_Occupy_t t;

    fd =fopen("/proc/stat","r");
    if (NULL == fd){
        return 0;
    }

    fgets(buff,sizeof(buff),fd);
    char name[64]={0};
    sscanf(buff,"%s %ld %ld %ld %ld",name,&t.user,&t.nice,&t.system,&t.idle);
    fclose(fd);
    //	printf("cpu-buff:%s \n",buff);
    return (t.user + t.nice + t.system + t.idle);
}
