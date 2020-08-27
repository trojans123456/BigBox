#ifndef __SYS_PORTING_H
#define __SYS_PORTING_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 接收来自发送方的字节
 * @param c
 * @param timeout
 * @return
 */
int32_t Receive_Byte(uint8_t *c,uint32_t timeout);

/**
 * @brief 发送一个字节
 * @param c
 * @return
 */
uint32_t Send_Byte(uint8_t c);

#ifdef __cplusplus
}
#endif

#endif
