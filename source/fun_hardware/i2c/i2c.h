#ifndef __I2C_H
#define __I2C_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct i2c__ i2c_t;

i2c_t *i2c_open(uint8_t i2c_bus,uint8_t i2c_address);

int i2c_close(i2c_t *i2c);

int i2c_write(i2c_t *i2c,uint8_t *buffer,uint16_t len);

int i2c_read(i2c_t *i2c,uint8_t *buffer,uint16_t len);

int i2c_set_timeout(i2c_t *i2c,int timeout);


#ifdef __cplusplus
}
#endif

#endif
