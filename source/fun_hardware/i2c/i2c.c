#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "i2c.h"

struct i2c__
{
    int fd;
    uint8_t bus;
    uint8_t address;
    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg message[2];
};


i2c_t *i2c_open(uint8_t i2c_bus, uint8_t i2c_address)
{
    i2c_t *i2c_dev = NULL;

    i2c_dev = calloc(1,sizeof(i2c_t));
    if(!i2c_dev)
        return NULL;

    i2c_dev->bus = i2c_bus;
    i2c_dev->address = i2c_address;

    char path[64] = "";
    sprintf(path,"/dev/i2c-%d",i2c_dev->bus);

    if(access(path,F_OK) != 0)
    {
        free(i2c_dev);
        return NULL;
    }

    i2c_dev->fd = open(path,O_SYNC | O_RDWR);
    if(i2c_dev->fd < 0)
    {
        free(i2c_dev);
        return NULL;
    }

    return i2c_dev;
}

int i2c_close(i2c_t *i2c)
{
    if(i2c == NULL)
        return -1;

    if(close(i2c->fd) < 0)
        return -1;

    free(i2c);

    return 0;
}

static int i2c_ioctl(i2c_t *i2c,int num_message)
{
    i2c->packets.msgs = i2c->message;
    i2c->packets.nmsgs = num_message;

    if(ioctl(i2c->fd,I2C_RDWR,&i2c->packets) < 0)
    {
        return -1;
    }

    return 0;
}

int i2c_write(i2c_t *i2c,uint8_t *buffer,uint16_t len)
{
    if(!i2c || !buffer)
        return -1;

    i2c->message[0].addr = i2c->address;
    i2c->message[0].flags = 0;
    i2c->message[0].len = len;
    i2c->message[0].buf = buffer;

    return i2c_ioctl(i2c,1);
}

/**
 * @brief i2c_read
 * @param i2c
 * @param buffer
 * @param len
 * @return
 */
int i2c_read(i2c_t *i2c,uint8_t *buffer,uint16_t len)
{
    if(!i2c || !buffer)
        return -1;

    i2c->message[0].addr = i2c->address;
    i2c->message[0].flags = I2C_M_RD;
    i2c->message[0].len = len;
    i2c->message[0].buf = buffer;

    return i2c_ioctl(i2c,1);
}

int i2c_set_timeout(i2c_t *i2c,int timeout)
{
    if(ioctl(i2c->fd,I2C_TIMEOUT,timeout) < 0)
    {
        return -1;
    }
    return 0;
}
