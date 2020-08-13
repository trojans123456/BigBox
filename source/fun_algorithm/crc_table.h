#ifndef __CRC_TABLE_H
#define __CRC_TABLE_H



#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


uint8_t crc8_tab(const uint8_t *input_str,int len);
uint16_t crc16_tab(const uint8_t *input_str,int len);
uint32_t crc32_tab(const uint8_t *input_str,int len);

#ifdef __cplusplus
}
#endif

#endif
