#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <net/if.h>
#include <unistd.h>

#include "can.h"

int can_open(const char *name)
{
    int fd;
    struct sockaddr_can addr;
    struct ifreq ifr;

    if((fd = socket(PF_CAN,SOCK_RAW,CAN_RAW)) < 0)
        return fd;

    //wish name
    strcpy(ifr.ifr_name,name);
    if((ioctl(fd,SIOCGIFINDEX,&ifr)) < 0) //Specify the name
    {
        return -1;
    }

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if((bind(fd,(struct sockaddr *)&addr,sizeof(addr))) < 0)
    {
        close(fd);
        return -1;
    }
    return fd;
}

void can_close(int fd)
{
    if(fd > 0)
        close(fd);
}

//禁止接收过滤规则，kernel会丢失掉该fd收到的can帧
void can_disable_recivefilter(int fd)
{
    setsockopt(fd,SOL_CAN_RAW,CAN_RAW_FILTER,NULL,0);
}

//禁止回环功能
void can_disable_loopback(int fd)
{
    int loopback = 0;
    setsockopt(fd,SOL_CAN_RAW,CAN_RAW_LOOPBACK,&loopback,sizeof(loopback));
}

void printf_errframe(struct can_frame *frame)
{
    if(frame->can_id & CAN_ERR_FLAG)
    {
        printf("%d \n",frame->can_id & (CAN_ERR_MASK | CAN_ERR_FLAG));
    }
}

int can_read(int fd,struct can_frame *frame,int cnt)
{
   return read(fd,frame,cnt);
}

int can_write(int fd,struct can_frame *frame)
{
   return  write(fd,frame,sizeof(struct can_frame));
}

// receive_canid & can_mask == can_id & can_mask
int can_set_filter(int fd,struct can_filter *filter,int count)
{
    setsockopt(fd,SOL_CAN_RAW,CAN_RAW_FILTER,filter,count);
}

/* 0-28 can_id 29 data or err frame 30 remote frme 31 std or exp frame
bit 0-28 : CAN identifier (11/29 bit)
 * bit 29 : error message frame flag (0 = data frame, 1 = error message)
 * bit 30 : remote transmission request flag (1 = rtr frame)
 * bit 31 : frame format flag (0 = standard 11 bit, 1 = extended 29 bit)
*/

/**
/* special address description flags for the CAN_ID
#define CAN_EFF_FLAG 0x80000000U /* EFF/SFF is set in the MSB
#define CAN_RTR_FLAG 0x40000000U /* remote transmission request
#define CAN_ERR_FLAG 0x20000000U /* error message frame

/* valid bits in CAN ID for frame formats
#define CAN_SFF_MASK 0x000007FFU /* standard frame format (SFF)
#define CAN_EFF_MASK 0x1FFFFFFFU /* extended frame format (EFF)
#define CAN_ERR_MASK 0x1FFFFFFFU /* omit EFF, RTR, ERR flags

 u32 sid, eid, exide, rtr;

 exide = (frame->can_id & CAN_EFF_FLAG) ? 1 : 0; //取can_id的31位，判断是标准帧还是扩展帧
 if (exide)
  sid = (frame->can_id & CAN_EFF_MASK) >> 18;//如果是扩展帧，can_id的0-28位为ID，其中高11位为标准ID
 else
  sid = frame->can_id & CAN_SFF_MASK; /* Standard ID
 eid = frame->can_id & CAN_EFF_MASK; /* Extended ID


 rtr = (frame->can_id & CAN_RTR_FLAG) ? 1 : 0; /* 是否是远程帧
**/
