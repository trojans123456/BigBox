#ifndef __I2C_H
#define __I2C_H

#include <stdint.h>
#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

declear_handler(hI2C);

hI2C i2cdev_open(const char *devname);
void i2cdev_close(hI2C i);


int i2c_read_data(hI2C i,uint16_t saddr,uint16_t regaddr,uint8_t *readbuf,uint16_t readlen);
int i2c_read_byte(hI2C i,uint16_t saddr,uint16_t regaddr, uint8_t data);
int i2c_write_data(hI2C i,uint16_t saddr,uint16_t regaddr,uint8_t *writebuf,uint16_t writelen);
int i2c_write_byte(hI2C i,uint16_t saddr,uint16_t regaddr,uint8_t data);

#ifdef __cplusplus
}
#endif

#endif // E_I2C_H_
