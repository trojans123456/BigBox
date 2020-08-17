#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <linux/spi/spidev.h>

#include "spi.h"

int spi_open(const char *name)
{
    return spi_open_advanced(name,SPI_MODE0,1000000,MSB_FIRST,8);
}
int spi_open_advanced(const char *name,unsigned int mode,unsigned int max_speed,
                      spi_bit_order_e bit_order,unsigned char bits_per_word)
{
    int fd = open(name,O_RDWR);
    if(fd < 0)
        return -1;

    spi_config_t cfg =
    {
        .spi_mode = mode,
        .spi_speed = max_speed,
        .spi_bit_per_word = bits_per_word,
        .spi_bit_order = bit_order
    };
    spi_setconfig(fd,&cfg);

    return fd;
}
void spi_close(int fd)
{
    if(fd > 0)
    {
        close(fd);
    }
}

void spi_setconfig(int fd,spi_config_t *cfg)
{
    if(!cfg)
        return ;
    unsigned char data;
    //set msb or lsb
    data = (cfg->spi_bit_order == LSB_FIRST ? SPI_LSB_FIRST : 0);
    if(ioctl(fd,SPI_IOC_WR_MODE,&data) < 0)
        return ;
    //set mode
    if(ioctl(fd,SPI_IOC_RD_MODE,&data) < 0)
        return ;
    data &= ~(SPI_CPOL |SPI_CPHA);
    data |= cfg->spi_mode;

    if(ioctl(fd,SPI_IOC_WR_MODE,&data) < 0)
        return ;

    //set bit per word
    if(ioctl(fd,SPI_IOC_WR_BITS_PER_WORD,&cfg->spi_bit_per_word) < 0)
        return ;
    //set speed
    if(ioctl(fd,SPI_IOC_WR_MAX_SPEED_HZ,&cfg->spi_speed) < 0)
        return ;
}

int spi_transfer(int fd,unsigned char *txbuf,unsigned char *rxbuf,int len)
{
    struct spi_ioc_transfer spi_xfer;

    memset(&spi_xfer,0x00,sizeof(struct spi_ioc_transfer));

    spi_xfer.tx_buf = txbuf;
    spi_xfer.rx_buf = rxbuf;
    spi_xfer.len = len;
    spi_xfer.delay_usecs = 0;
    spi_xfer.speed_hz = 0;
    spi_xfer.bits_per_word = 0;
    spi_xfer.cs_change = 0;

    //transfer
    if(ioctl(fd,SPI_IOC_MESSAGE(1),&spi_xfer) < 0)
        return -1;

    return 0;
}
