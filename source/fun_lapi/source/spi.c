#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#ifdef __linux__
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <linux/spi/spidev.h> /* (SPI_CPHA | SPI_CPOL) */
#endif
#include "spi.h"

typedef struct spi
{
    int fd;
}spi_t;


hSPI spi_open_advanced(const char *path, unsigned int mode, uint32_t max_speed, spi_bit_order_t bit_order, uint8_t bits_per_word, uint8_t extra_flags) {
    uint8_t data8;

    spi_t *spi = (spi_t *)calloc(1,sizeof(spi_t));
    if(!spi)
        return NULL;

    /* Validate arguments */
    if (mode & ~0x3)
    {
        free(spi);
        return NULL;
    }
    if (bit_order != MSB_FIRST && bit_order != LSB_FIRST)
    {
        free(spi);
        return NULL;
    }


    /* Open device */
    if ((spi->fd = open(path, O_RDWR)) < 0)
    {
        free(spi);
        return NULL;
    }

    /* Set mode, bit order, extra flags */
    data8 = mode | ((bit_order == LSB_FIRST) ? SPI_LSB_FIRST : 0) | extra_flags;
    if (ioctl(spi->fd, SPI_IOC_WR_MODE, &data8) < 0) {
        close(spi->fd);
        free(spi);
        return NULL;
    }

    /* Set max speed */
    if (ioctl(spi->fd, SPI_IOC_WR_MAX_SPEED_HZ, &max_speed) < 0) {
        close(spi->fd);
        free(spi);
        return NULL;
    }

    /* Set bits per word */
    if (ioctl(spi->fd, SPI_IOC_WR_BITS_PER_WORD, &bits_per_word) < 0) {
        close(spi->fd);
        free(spi);
        return NULL;
    }

    return (hSPI)spi;
}

hSPI spi_open(const char *path, unsigned int mode, uint32_t max_speed) {
    return spi_open_advanced(path, mode, max_speed, MSB_FIRST, 8, 0);
}


int spi_transfer(hSPI s, const uint8_t *txbuf, uint8_t *rxbuf, size_t len) {
    struct spi_ioc_transfer spi_xfer;
    spi_t *spi = (spi_t *)s;
    if(!spi)
        return -1;

    /* Prepare SPI transfer structure */
    memset(&spi_xfer, 0, sizeof(struct spi_ioc_transfer));
    spi_xfer.tx_buf = (__u64)txbuf;
    spi_xfer.rx_buf = (__u64)rxbuf;
    spi_xfer.len = len;
    spi_xfer.delay_usecs = 0;
    spi_xfer.speed_hz = 0;
    spi_xfer.bits_per_word = 0;
    spi_xfer.cs_change = 0;

    /* Transfer */
    if (ioctl(spi->fd, SPI_IOC_MESSAGE(1), &spi_xfer) < 1)
        return -1;

    return 0;
}

int spi_close(hSPI s) {
    spi_t *spi = (spi_t *)s;
    if(!spi)
        return -1;

    if (spi->fd < 0)
        return -1;

    /* Close fd */
    if (close(spi->fd) < 0)
        return -1;

    spi->fd = -1;
    free(spi);

    return 0;
}

int spi_get_mode(hSPI s, unsigned int *mode) {
    uint8_t data8;
    spi_t *spi = (spi_t *)s;
    if(!spi)
        return -1;

    if (ioctl(spi->fd, SPI_IOC_RD_MODE, &data8) < 0)
        return -1;

    *mode = data8 & (SPI_CPHA | SPI_CPOL);

    return 0;
}

int spi_get_max_speed(hSPI s, uint32_t *max_speed) {
    uint32_t data32;
    spi_t *spi = (spi_t *)s;
    if(!spi)
        return -1;

    if (ioctl(spi->fd, SPI_IOC_RD_MAX_SPEED_HZ, &data32) < 0)
        return -1;

    *max_speed = data32;

    return 0;
}

int spi_get_bit_order(hSPI s, spi_bit_order_t *bit_order) {
    uint8_t data8;
    spi_t *spi = (spi_t *)s;
    if(!spi)
        return -1;

    if (ioctl(spi->fd, SPI_IOC_RD_LSB_FIRST, &data8) < 0)
        return -1;

    if (data8)
        *bit_order = LSB_FIRST;
    else
        *bit_order = MSB_FIRST;

    return 0;
}

int spi_get_bits_per_word(hSPI s, uint8_t *bits_per_word) {
    uint8_t data8;
    spi_t *spi = (spi_t *)s;
    if(!spi)
        return -1;

    if (ioctl(spi->fd, SPI_IOC_RD_BITS_PER_WORD, &data8) < 0)
        return -1;

    *bits_per_word = data8;

    return 0;
}

int spi_get_extra_flags(hSPI s, uint8_t *extra_flags) {
    uint8_t data8;
    spi_t *spi = (spi_t *)s;
    if(!spi)
        return -1;

    if (ioctl(spi->fd, SPI_IOC_RD_MODE, &data8) < 0)
        return -1;

    /* Extra mode flags without mode 0-3 and bit order */
    *extra_flags = data8 & ~( SPI_CPOL | SPI_CPHA | SPI_LSB_FIRST );

    return 0;
}

int spi_set_mode(hSPI s, unsigned int mode) {
    uint8_t data8;
    spi_t *spi = (spi_t *)s;
    if(!spi)
        return -1;

    if (mode & ~0x3)
        return -1;

    if (ioctl(spi->fd, SPI_IOC_RD_MODE, &data8) < 0)
        return -1;

    data8 &= ~(SPI_CPOL | SPI_CPHA);
    data8 |= mode;

    if (ioctl(spi->fd, SPI_IOC_WR_MODE, &data8) < 0)
        return -1;

    return 0;
}

int spi_set_bit_order(hSPI s, spi_bit_order_t bit_order) {
    uint8_t data8;
    spi_t *spi = (spi_t *)s;
    if(!spi)
        return -1;

    if (bit_order != MSB_FIRST && bit_order != LSB_FIRST)
        return -1;

    if (bit_order == LSB_FIRST)
        data8 = 1;
    else
        data8 = 0;

    if (ioctl(spi->fd, SPI_IOC_WR_LSB_FIRST, &data8) < 0)
        return -1;

    return 0;
}

int spi_set_extra_flags(hSPI s, uint8_t extra_flags) {
    uint8_t data8;
    spi_t *spi = (spi_t *)s;
    if(!spi)
        return -1;

    if (ioctl(spi->fd, SPI_IOC_RD_MODE, &data8) < 0)
        return -1;

    /* Keep mode 0-3 and bit order */
    data8 &= (SPI_CPOL | SPI_CPHA | SPI_LSB_FIRST);
    /* Set extra flags */
    data8 |= extra_flags;

    if (ioctl(spi->fd, SPI_IOC_WR_MODE, &data8) < 0)
        return -1;

    return 0;
}

int spi_set_max_speed(hSPI s, uint32_t max_speed) {
    spi_t *spi = (spi_t *)s;
    if(!spi)
        return -1;

    if (ioctl(spi->fd, SPI_IOC_WR_MAX_SPEED_HZ, &max_speed) < 0)
        return -1;

    return 0;
}

int spi_set_bits_per_word(hSPI s, uint8_t bits_per_word) {
    spi_t *spi = (spi_t *)s;
    if(!spi)
        return -1;

    if (ioctl(spi->fd, SPI_IOC_WR_BITS_PER_WORD, &bits_per_word) < 0)
        return -1;

    return 0;
}

#if 0
int spi_tostring(hSPI s, char *str, size_t len) {
    unsigned int mode;
    char mode_str[2];
    uint32_t max_speed;
    char max_speed_str[16];
    uint8_t bits_per_word;
    char bits_per_word_str[2];
    spi_bit_order_t bit_order;
    char bit_order_str[16];
    uint8_t extra_flags;
    char extra_flags_str[4];

    spi_t *spi = (spi_t *)s;
    if(!spi)
        return -1;

    if (spi_get_mode(spi, &mode) < 0)
        strncpy(mode_str, "?", sizeof(mode_str));
    else
        snprintf(mode_str, sizeof(mode_str), "%d", mode);

    if (spi_get_max_speed(spi, &max_speed) < 0)
        strncpy(max_speed_str, "?", sizeof(max_speed_str));
    else
        snprintf(max_speed_str, sizeof(max_speed_str), "%u", max_speed);

    if (spi_get_bit_order(spi, &bit_order) < 0)
        strncpy(bit_order_str, "?", sizeof(bit_order_str));
    else
        strncpy(bit_order_str, (bit_order == LSB_FIRST) ? "LSB first" : "MSB first", sizeof(bit_order_str));

    if (spi_get_bits_per_word(spi, &bits_per_word) < 0)
        strncpy(bits_per_word_str, "?", sizeof(bits_per_word_str));
    else
        snprintf(bits_per_word_str, sizeof(bits_per_word_str), "%u", bits_per_word);

    if (spi_get_extra_flags(spi, &extra_flags) < 0)
        strncpy(extra_flags_str, "?", sizeof(extra_flags_str));
    else
        snprintf(extra_flags_str, sizeof(extra_flags_str), "%02x", extra_flags);

    return snprintf(str, len, "SPI (fd=%d, mode=%s, max_speed=%s, bit_order=%s, bits_per_word=%s, extra_flags=%s)", spi->fd, mode_str, max_speed_str, bit_order_str, bits_per_word_str, extra_flags_str);
}
#endif

