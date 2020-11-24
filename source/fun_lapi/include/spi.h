#ifndef __SPI_H
#define __SPI_H

#include <stdint.h>
#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

declear_handler(hSPI);

/**
  spidev.h
  #define SPI_CPHA 0x01
  #define SPI_CPHA 0x02
*/
typedef enum
{
    SPI_MODE0 = 0,
    SPI_MODE1 = (0 | 0x01),
    SPI_MODE2 = (0x02 | 0),
    SPI_MODE3 = (0x01 | 0x02)
}spi_mode_t;  /* for mode */

typedef enum
{
    MSB_FIRST,
    LSB_FIRST
}spi_bit_order_t;

/* Primary Functions  path = /dev/spidev%d.%d*/
hSPI spi_open(const char *path, unsigned int mode,
                uint32_t max_speed);


int spi_transfer(hSPI s,const uint8_t *txbuf, uint8_t *rxbuf, size_t len);
int spi_close(hSPI s);

/* Getters */
int spi_get_mode(hSPI s, unsigned int *mode);
int spi_get_max_speed(hSPI s, uint32_t *max_speed);
int spi_get_bit_order(hSPI s, spi_bit_order_t *bit_order);
int spi_get_bits_per_word(hSPI s, uint8_t *bits_per_word);
int spi_get_extra_flags(hSPI s, uint8_t *extra_flags);

/* Setters */
int spi_set_mode(hSPI s, unsigned int mode);
int spi_set_max_speed(hSPI s, uint32_t max_speed);
int spi_set_bit_order(hSPI s, spi_bit_order_t bit_order);
int spi_set_bits_per_word(hSPI s, uint8_t bits_per_word);
int spi_set_extra_flags(hSPI s, uint8_t extra_flags);

/* Miscellaneous */
//int spi_fd(hSPI s);



#ifdef __cplusplus
}
#endif

#endif
