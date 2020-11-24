#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#ifdef __linux__
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#endif

#include "i2c.h"

typedef struct i2c
{
    int fd;
    uint64_t supported; /*所支持的功能*/
}i2c_t;

hI2C i2cdev_open(const char *devname)
{
    i2c_t *i2c = (i2c_t *)calloc(1,sizeof(i2c_t));
    if(!i2c)
        return NULL;

    i2c->fd = open(devname,O_RDWR);
    if(i2c->fd < 0)
    {
        free(i2c);
        return NULL;
    }

    /* 获取支持的功能*/
    if(ioctl(i2c->fd,I2C_FUNCS,&i2c->supported) < 0)
    {
        close(i2c->fd);
        free(i2c);
        return NULL;
    }
    return (hI2C)i2c;
}

void i2cdev_close(hI2C i)
{
    i2c_t *i2c = (i2c_t *)i;
    if(!i2c)
        return ;

    if(i2c->fd > 0)
    {
        close(i2c->fd);
    }
    free(i2c);
}

static int i2c_transfer(int fd,struct i2c_msg *msgs,int msg_cnt)
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

int i2c_write_byte(hI2C i,uint16_t saddr,uint16_t regaddr,uint8_t data)
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
    return i2c_write_data(i,saddr,regaddr,&data,1);
}

int i2c_write_data(hI2C i,uint16_t saddr,uint16_t regaddr,uint8_t *writebuf,uint16_t writelen)
{
    i2c_t *i2c = (i2c_t *)i;
    if(!i2c)
        return -1;

    struct i2c_msg msg =
    {
        .addr = saddr,
        .flags = 0,
        .len = writelen,
        .buf = writebuf
    };

    ioctl(i2c->fd,I2C_TIMEOUT,2);//超时时间 2s
    ioctl(i2c->fd,I2C_RETRIES,1);//设置重发次数

    return i2c_transfer(i2c->fd,&msg,1);
}
//saddr slave addr 设备地址。写入寄存器或eeprom地址
int i2c_read_byte(hI2C i,uint16_t saddr,uint16_t regaddr,uint8_t data)
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
    return i2c_read_data(i,saddr,regaddr,&data,1);
}

int i2c_read_data(hI2C i,unsigned short saddr,unsigned short regaddr,unsigned char *readbuf,unsigned short readlen)
{
    i2c_t *i2c = (i2c_t *)i;
    if(!i2c)
        return -1;

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

    ioctl(i2c->fd,I2C_TIMEOUT,2);//超时时间 2s
    ioctl(i2c->fd,I2C_RETRIES,1);//设置重发次数

    return i2c_transfer(i2c->fd,msgs,2);
}
