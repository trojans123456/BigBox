/**
# 数据帧 发送 节点到节点的数据

# 远程帧 接收 节点具有相同ID的发送节点传送数据的帧

# 错误帧

# 过载帧  某个节点尚未做好准备的帧

# 帧间隔

#    标准帧  127bit
#----------------------------------------
#|     |<- 仲裁域   ->|<- 控制域      ->| 数据域|    CRC                   ->|
#| SOF | 标识符 | RTR | IDE | R0 | DLC | data | CRC | crc分隔符 | ACK | EOF |
#-------------------------------------------------------------------------------
#| 1   |  11   |  1  |  1  | 1  |  4  | 0-64 | 15  |  1       |  2  |  7  |
#-----------------------------------------------------------------------------
#
#| SOF | 标识符 | SRR | IDE | 扩展 | R1 | R0 | DLC | ----
#|  1  |  11   |  1  |  1  | 18   | 1 |  1  |  4
#

# RTR : 远程发送请求位  数据帧为 显性  远程帧为隐性
# SRR ： 代替远程请求位 = 0 因为在扩展帧的再RTR的位置
# IDE ： 标识符扩展位  标准显性  扩展帧为隐性

R0 R1保留位
*/
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
