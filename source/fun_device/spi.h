#ifndef __SPI_H_
#define __SPI_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    MSB_FIRST,
    LSB_FIRST
}spi_bit_order_e;

typedef enum
{
    SPI_MODE0,
    SPI_MODE1,
    SPI_MODE2,
    SPI_MODE3
}spi_mode_e;

typedef struct
{
    unsigned int spi_speed;
    unsigned int spi_bit_per_word;
    unsigned int spi_bit_order;
    unsigned char spi_mode;
}spi_config_t;

int spi_open(const char *name);
int spi_open_advanced(const char *name,unsigned int mode,unsigned int max_speed,
                      spi_bit_order_e bit_order,unsigned char bits_per_word);
void spi_close(int fd);

void spi_setconfig(int fd,spi_config_t *cfg);

int spi_transfer(int fd,unsigned char *txbuf,unsigned char *rxbuf,int len);

#ifdef __cplusplus
}
#endif

#endif // E_SPI_H_
