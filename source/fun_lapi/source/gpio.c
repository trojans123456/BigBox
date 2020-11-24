#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#ifdef __linux__
#include <poll.h>
#endif
#include <sys/stat.h>

#include "gpio.h"

typedef struct
{
    uint32_t pin;
    int fd;
}gpio_t;

static const char *gpio_dir2str[] =
{
    "in","out","low","high"
};

static const char *gpio_edge2str[] =
{
    "none","rising","falling","both"
};

#define PATH_MAX    256
/* 查询 export是否成功的间隔 100ms*/
#define GPIO_EXPORT_STAT_DELAY      100000
/* export导出重试次数 */
#define GPIO_EXPORT_STAT_RETRIES    10

hGPIO gpio_open(uint32_t pin,gpio_direction_e dir)
{
    char gpio_path[PATH_MAX];
    struct stat stat_buf;
    char buf[16];
    int fd;
    gpio_t *gpio = NULL;

    if(dir > GPIO_DIR_PRESERVE)
        return NULL;

    gpio = (gpio_t *)calloc(1,sizeof(gpio_t));
    if(!gpio)
        return NULL;

    snprintf(gpio_path,sizeof(gpio_path),"/sys/class/gpio/gpio%d", pin);
    if(stat(gpio_path,&stat_buf) < 0)
    {
        /*不存在*/
        snprintf(buf,sizeof(buf),"%d",pin);
        if((fd = open("/sys/class/gpio/export", O_WRONLY)) < 0)
        {
            free(gpio);
            return NULL;
        }

        if(write(fd,buf,strlen(buf) + 1) < 0)
        {
            close(fd);
            free(gpio);
            return NULL;
        }

        if(close(fd) < 0)
        {
            free(gpio);
            return NULL;
        }

        /* Wait until GPIO directory appears */
        unsigned int retry_count;
        for (retry_count = 0; retry_count < GPIO_EXPORT_STAT_RETRIES; retry_count++) {
            int ret = stat(gpio_path, &stat_buf);
            if (ret == 0)
                break;
            else if (ret < 0 && errno != ENOENT)
            {
                free(gpio);
                return NULL;
            }


            usleep(GPIO_EXPORT_STAT_DELAY);
        }

        if(retry_count == GPIO_EXPORT_STAT_RETRIES)
        {
            free(gpio);
            return NULL;
        }

    }

    /*如果不保持现有的方向*/
    if(dir != GPIO_DIR_PRESERVE)
    {
        snprintf(gpio_path, sizeof(gpio_path), "/sys/class/gpio/gpio%d/direction", pin);
        if ((fd = open(gpio_path, O_WRONLY)) < 0)
        {
            free(gpio);
            return NULL;
        }

        if (write(fd, gpio_dir2str[dir], strlen(gpio_dir2str[dir])+1) < 0) {
            close(fd);
            free(gpio);
            return NULL;
        }
        if (close(fd) < 0)
        {
            free(gpio);
            return NULL;
        }
    }

    memset(gpio,0,sizeof(gpio_t));
    gpio->pin = pin;

    /* open pin */
    snprintf(gpio_path, sizeof(gpio_path), "/sys/class/gpio/gpio%d/value", pin);
    if ((gpio->fd = open(gpio_path, O_RDWR)) < 0)
    {
        free(gpio);
        return NULL;
    }

    return (hGPIO)gpio;
}

int gpio_close(hGPIO g)
{
    gpio_t *gpio = (gpio_t *)g;

    if(!gpio || gpio->fd < 0)
        return -1;

    if(close(gpio->fd) < 0)
        return -1;

    gpio->fd = -1;

    return 0;
}

int gpio_read(hGPIO g,uint8_t *value)
{
    char buf[2];
    gpio_t *gpio = (gpio_t *)g;
    if(!gpio)
        return -1;

    /* read */
    if(read(gpio->fd,buf,2) < 0)
        return -1;

    /* rewind */
    if(lseek(gpio->fd,0,SEEK_SET) < 0)
        return -1;

    if(buf[0] == '0')
        *value = 0;
    else if(buf[0] == '1')
        *value = 1;
    else
        return -1;

    return 0;
}

int gpio_write(hGPIO g,uint8_t value)
{
    char value_str[][2] = {"0","1"};
    gpio_t *gpio = (gpio_t *)g;
    if(!gpio)
        return -1;

    if(write(gpio->fd,value_str[value],2) < 0)
        return -1;

    if(lseek(gpio->fd,0,SEEK_SET) < 0)
        return -1;

    return 0;
}

int gpio_poll(hGPIO g,int timeout_ms)
{
    struct pollfd fds[1];
    char buf[1];
    int ret = -1;
    gpio_t *gpio = (gpio_t *)g;
    if(!gpio)
        return -1;

    /* dummy read befor poll */
    if(read(gpio->fd,buf,1) < 0)
        return -1;

    /* seek to end */
    if(lseek(gpio->fd,0,SEEK_SET) < 0)
        return -1;

    /* poll */
    fds[0].fd = gpio->fd;
    fds[0].events = POLLPRI | POLLERR;
    if((ret == poll(fds,1,timeout_ms)) < 0)
        return -1;
    if(ret)
    {
        /* rewind */
        if(lseek(gpio->fd,0,SEEK_SET) < 0)
            return -1;
        return 1;
    }

    /* timeout */
    return 0;
}


int gpio_supports_interrupts(hGPIO g,uint8_t *supported)
{
    char gpio_path[PATH_MAX];
    struct stat stat_buf;

    gpio_t *gpio = (gpio_t *)g;
    if(!gpio)
        return -1;

    /* Check for edge */
    snprintf(gpio_path, sizeof(gpio_path), "/sys/class/gpio/gpio%d/edge", gpio->pin);

    if (stat(gpio_path, &stat_buf) < 0) {
        if (errno == ENOENT) {
            *supported = 0;
            return 0;
        }

        /* Other error */
        return -1;
    }

    *supported = 1;
    return 0;
}

int gpio_get_direction(hGPIO g,gpio_direction_e *direction)
{
    char gpio_path[PATH_MAX];
    char buf[8];
    int fd, ret;

    gpio_t *gpio = (gpio_t *)g;
    if(!gpio)
        return -1;

    /* Read direction */
    snprintf(gpio_path, sizeof(gpio_path), "/sys/class/gpio/gpio%d/direction", gpio->pin);
    if ((fd = open(gpio_path, O_RDONLY)) < 0)
        return -1;
    if ((ret = read(fd, buf, sizeof(buf))) < 0) {

        close(fd);
        return -1;
    }
    if (close(fd) < 0)
        return -1;

    buf[ret] = '\0';

    if (strcmp(buf, "in\n") == 0)
        *direction = GPIO_DIR_IN;
    else if (strcmp(buf, "out\n") == 0)
        *direction = GPIO_DIR_OUT;
    else
        return -1;

    return 0;
}

int gpio_get_edge(hGPIO g,gpio_edge_e *edge)
{
    char gpio_path[PATH_MAX];
    char buf[16];
    int fd, ret;

    gpio_t *gpio = (gpio_t *)g;
    if(!gpio)
        return -1;

    /* Read edge */
    snprintf(gpio_path, sizeof(gpio_path), "/sys/class/gpio/gpio%d/edge", gpio->pin);
    if ((fd = open(gpio_path, O_RDONLY)) < 0)
        return -1;
    if ((ret = read(fd, buf, sizeof(buf))) < 0) {

        close(fd);
        return -1;
    }
    if (close(fd) < 0)
        return -1;

    buf[ret] = '\0';

    if (strcmp(buf, "none\n") == 0)
        *edge = GPIO_EDGE_NONE;
    else if (strcmp(buf, "rising\n") == 0)
        *edge = GPIO_EDGE_RISING;
    else if (strcmp(buf, "falling\n") == 0)
        *edge = GPIO_EDGE_FALLING;
    else if (strcmp(buf, "both\n") == 0)
        *edge = GPIO_EDGE_BOTH;
    else
        return -1;

    return 0;
}

int gpio_set_direction(hGPIO g,gpio_direction_e direction)
{
    char gpio_path[PATH_MAX];
    int fd;

    gpio_t *gpio = (gpio_t *)g;
    if(!gpio)
        return -1;

    if (direction != GPIO_DIR_IN && direction != GPIO_DIR_OUT && direction != GPIO_DIR_OUT_LOW && direction != GPIO_DIR_OUT_HIGH)
        return -1;

    /* Write direction */
    snprintf(gpio_path, sizeof(gpio_path), "/sys/class/gpio/gpio%d/direction", gpio->pin);
    if ((fd = open(gpio_path, O_WRONLY)) < 0)
        return -1;
    if (write(fd, gpio_dir2str[direction], strlen(gpio_dir2str[direction])+1) < 0) {

        close(fd);
        return -1;
    }
    if (close(fd) < 0)
        return -1;

    return 0;
}

int gpio_set_edge(hGPIO g, gpio_edge_e edge)
{
    char gpio_path[PATH_MAX];
    int fd;

    gpio_t *gpio = (gpio_t *)g;
    if(!gpio)
        return -1;

    if (edge != GPIO_EDGE_NONE && edge != GPIO_EDGE_RISING && edge != GPIO_EDGE_FALLING && edge != GPIO_EDGE_BOTH)
        return -1;

    /* Write edge */
    snprintf(gpio_path, sizeof(gpio_path), "/sys/class/gpio/gpio%d/edge", gpio->pin);
    if ((fd = open(gpio_path, O_WRONLY)) < 0)
        return -1;
    if (write(fd, gpio_edge2str[edge], strlen(gpio_edge2str[edge])+1) < 0) {

        close(fd);
        return -1;
    }
    if (close(fd) < 0)
        return -1;

    return 0;
}
