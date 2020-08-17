#ifndef __FIFO_H
#define __FIFO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct fifo__ fifo_t;

#ifndef MIN
#define MIN(x,y)    ((x) < (y) ? (x) : (y))
#endif

/**
 * @brief ring_buffer_init
 * @param ring_buff
 * @param buffer
 * @param size
 * @return
 */
int ring_buffer_init(fifo_t *ring_buff,void *buffer,uint32_t size);

/**
 * @brief ring_buffer_deinit
 * @param ring_buff
 * @return
 */
int ring_buffer_deinit(fifo_t *ring_buff);

/**
 * @brief ring_buffer_free_space
 * @param ring_buff
 * @return
 */
uint32_t ring_buffer_free_space(fifo_t *ring_buff);

/**
 * @brief ring_buffer_used_space
 * @param ring_buff
 * @return
 */
uint32_t ring_buffer_used_space(fifo_t *ring_buff);

/**
 * @brief ring_buffer_get_data
 * @param ring_buff
 * @param data
 * @param contiguous_bytes
 * @return
 */
int ring_buffer_get_data(fifo_t *ring_buff,uint8_t** data, uint32_t* contiguous_bytes);

/**
 * @brief ring_buffer_consume
 * @param ring_buff
 * @param bytes_consumed
 * @return
 */
int ring_buffer_consume(fifo_t *ring_buff,uint32_t bytes_consumed);

/**
 * @brief ring_buffer_write
 * @param ring_buffer
 * @param data
 * @param data_length
 * @return
 */
uint32_t ring_buffer_write( fifo_t* ring_buffer, const uint8_t* data, uint32_t data_length );

/**
 * @brief ring_buffer_is_full
 * @param ring_buffer
 * @return
 */
uint8_t ring_buffer_is_full(fifo_t *ring_buffer);

/**
 * @brief ring_buffer_read
 * @param ring_buffer
 * @param data
 * @param data_length
 * @param number_of_bytes_read
 * @return
 */
int ring_buffer_read( fifo_t* ring_buffer, uint8_t* data, uint32_t data_length, uint32_t* number_of_bytes_read );

/**
 * @brief ring_buffer_total_size
 * @param ring_buffer
 * @return
 */
uint32_t ring_buffer_total_size(fifo_t *ring_buffer);

#ifdef __cplusplus
}
#endif

#endif
