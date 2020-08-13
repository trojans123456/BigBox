#ifndef __CRC_H
#define __CRC_H

#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif



uint8_t crc8(uint8_t *input,int len);

uint16_t crc16(uint8_t *input,int len);

uint32_t crc32(uint8_t *input,int len);


#ifdef __cplusplus
}
#endif

#endif
