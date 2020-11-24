#ifndef __GPIO_H
#define __GPIO_H

#include <stdint.h>

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

declear_handler(hGPIO);

typedef enum gpio_direction {
    GPIO_DIR_IN = 0,        /* Input */
    GPIO_DIR_OUT,       /* Output, initialized to low */
    GPIO_DIR_OUT_LOW,   /* Output, initialized to low */
    GPIO_DIR_OUT_HIGH,  /* Output, initialized to high */
    GPIO_DIR_PRESERVE,  /* Preserve existing direction */
}gpio_direction_e;

typedef enum gpio_edge {
    GPIO_EDGE_NONE,     /* No interrupt edge */
    GPIO_EDGE_RISING,   /* Rising edge 0 -> 1 */
    GPIO_EDGE_FALLING,  /* Falling edge 1 -> 0 */
    GPIO_EDGE_BOTH      /* Both edges X -> !X */
}gpio_edge_e;

hGPIO gpio_open(uint32_t pin,gpio_direction_e dir);

int gpio_close(hGPIO gpio);

int gpio_read(hGPIO gpio,uint8_t *value);

int gpio_write(hGPIO gpio,uint8_t value);

int gpio_poll(hGPIO gpio,int timeout_ms);


int gpio_supports_interrupts(hGPIO gpio,uint8_t *support);

int gpio_get_direction(hGPIO gpio,gpio_direction_e *dir);

int gpio_get_edge(hGPIO gpio, gpio_edge_e *edge);

int gpio_set_direction(hGPIO gpio,gpio_direction_e dir);

int gpio_set_edge(hGPIO gpio, gpio_edge_e edge);

#ifdef __cplusplus
}
#endif

#endif
