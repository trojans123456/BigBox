#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "i2c.h"

int i2cdev_open(const char *devname)
{
    int fd = open(devname,O_RDWR);
    if(fd < 0)
        return -1;

    /* 获取支持的功能*/
    unsigned long supported;
    if(ioctl(fd,I2C_FUNCS,&supported) < 0)
    {
        close(fd);
        return -1;
    }
    return fd;
}

void i2cdev_close(int fd)
{
    if(fd > 0)
    {
        close(fd);
    }
}

int i2c_transfer(int fd,struct i2c_msg *msgs,int msg_cnt)
{
    struct i2c_rdwr_ioctl_data i2c_rdwr_data;

    memset(&i2c_rdwr_data,0x00,sizeof(struct i2c_rdwr_ioctl_data));

    i2c_rdwr_data.msgs = msgs;
    i2c_rdwr_data.nmsgs = msg_cnt;

    if(ioctl(fd,I2C_RDWR,&i2c_rdwr_data) < 0)
    {
        return -1;
    }
    return 0;
}

int i2c_write_byte(int fd,unsigned char saddr,unsigned short regaddr,unsigned char data)
{
#if 0
    struct i2c_msg msg =
    {
        .addr = saddr,
        .flags = 0,
        .len = 1,
        .buf = &data
    };

    ioctl(fd,I2C_TIMEOUT,2);//超时时间 2s
    ioctl(fd,I2C_RETRIES,1);//设置重发次数

    return i2c_transfer(fd,&msg,1);
#endif
    return i2c_write_data(fd,saddr,regaddr,&data,1);
}

int i2c_write_data(int fd,unsigned short saddr,unsigned short regaddr,unsigned char *writebuf,unsigned short writelen)
{
    struct i2c_msg msg =
    {
        .addr = saddr,
        .flags = 0,
        .len = writelen,
        .buf = writebuf
    };

    ioctl(fd,I2C_TIMEOUT,2);//超时时间 2s
    ioctl(fd,I2C_RETRIES,1);//设置重发次数

    return i2c_transfer(fd,&msg,1);
}
//saddr slave addr 设备地址。写入寄存器或eeprom地址
int i2c_read_byte(int fd,unsigned short saddr,unsigned short regaddr,unsigned char data)
{
#if 0
    struct i2c_msg msgs[2] =
    {
        {
            .addr = saddr,
            .flags = 0,//write
            .len = 2,
            .buf = &regaddr
        },
        {
            .addr = saddr,
            .flags = I2C_M_RD,//read
            .len = 1,
            .buf = &data
        }
    };

    ioctl(fd,I2C_TIMEOUT,2);//超时时间 2s
    ioctl(fd,I2C_RETRIES,1);//设置重发次数

    return i2c_transfer(fd,msgs,2);
#endif
    return i2c_read_data(fd,saddr,regaddr,&data,1);
}

int i2c_read_data(int fd,unsigned short saddr,unsigned short regaddr,unsigned char *readbuf,unsigned short readlen)
{
    struct i2c_msg msgs[2] =
    {
        {
            .addr = saddr,
            .flags = 0,//write
            .len = 2,
            .buf = (unsigned char *)&regaddr
        },
        {
            .addr = saddr,
            .flags = I2C_M_RD,//read
            .len = readlen,
            .buf = readbuf
        }
    };

    ioctl(fd,I2C_TIMEOUT,2);//超时时间 2s
    ioctl(fd,I2C_RETRIES,1);//设置重发次数

    return i2c_transfer(fd,msgs,2);
}
