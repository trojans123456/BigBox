#ifndef __GPIO_H_
#define __GPIO_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    EDGE_NONE,
    EDGE_RISING,
    EDGE_FALLING,
    EDGE_BOTH
}edge_e;

typedef enum
{
    FLAG_OUT,
    FLAG_IN,
    FLAG_HIGH
}dir_t;

int gpio_export(unsigned int gpio);
int gpio_unexport(unsigned int gpio);
int gpio_set_dir(unsigned int gpio,int out_flag);
int gpio_get_dir(unsigned int gpio,int *out_flag);
int gpio_set_value(unsigned int gpio,unsigned int value);
int gpio_get_value(unsigned int gpio,unsigned int *value);

int gpio_set_edge(unsigned int gpio,unsigned int edge_flag);

#ifdef __cplusplus
}
#endif

#endif // E_GPIO_H_
