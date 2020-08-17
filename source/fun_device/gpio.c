#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "gpio.h"


#define GPIO_SYS_PATH "/sys/class/gpio"
#define GPIO_MAX_BUF 60



int gpio_export(unsigned int gpio)
{
    char buf[GPIO_MAX_BUF] = "";
    int fd,len;

    fd = open(GPIO_SYS_PATH "/export",O_WRONLY);
    if(fd < 0){
        printf("open export failed\n");
        return -1;
    }
    len = snprintf(buf,sizeof(buf),"%d",gpio);
    if(len < 0){
        printf("write gpio number to buf failed\n");
        return -1;
    }
    write(fd,buf,len);
    close(fd);

    return 0;
}

int gpio_unexport(unsigned int gpio)
{
    int fd,len;
    char buf[GPIO_MAX_BUF] = "";

    fd = open(GPIO_SYS_PATH "/unexport",O_WRONLY);
    if(fd < 0){
        printf("open unexport failed\n");
        return -1;
    }

    len = snprintf(buf,sizeof(buf),"%d",gpio);
    if(len < 0){
        printf("write gpio number to buf failed\n");
        return -1;
    }
    write(fd,buf,len);
    close(fd);

    return 0;
}

int gpio_set_dir(unsigned int gpio,int out_flag)
{
    int fd,len;
    char buf[GPIO_MAX_BUF] = "";

    len = snprintf(buf,sizeof(buf),GPIO_SYS_PATH "/gpio%d/direction",gpio);
    if(len < 0){
        printf("write gpio number to buf failed\n");
        return -1;
    }
    fd = open(buf,O_WRONLY);
    if(fd < 0){
        printf("open gpio%d failed\n",gpio);
        return -1;
    }

    switch(out_flag){
        case FLAG_OUT:
            write(fd,"out",4);
            break;
        case FLAG_IN:
            write(fd,"in",3);
            break;
        case FLAG_HIGH:
            write(fd,"high",5);
            break;
        default:
            printf("Unkown %d flag type\n",out_flag);
            return -1;
    }
    close(fd);
    return 0;
}

int gpio_set_value(unsigned int gpio,unsigned int value)
{
    int fd,len;
    char buf[GPIO_MAX_BUF] = "";

    len = snprintf(buf,sizeof(buf),"/gpio%d/value",gpio);
    if(len < 0){
        printf("write gpio%d number to buf failed\n",gpio);
        return -1;
    }

    fd = open(buf,O_WRONLY);
    if(fd < 0){
        printf("open gpio%d failed\n",gpio);
        return -1;
    }

    if(value){
        write(fd,"1",2);
    }else{
        write(fd,"0",2);
    }
    close(fd);
    return 0;
}

int gpio_get_value(unsigned int gpio,unsigned int *value)
{
    int fd,len;
    char tmp_value;
    char buf[GPIO_MAX_BUF] = "";

    len = snprintf(buf,sizeof(buf),GPIO_SYS_PATH "/gpio%d/value",gpio);
    if(len < 0){
        printf("write gpio%d number to buf failed\n",gpio);
        return -1;
    }

    fd = open(buf,O_RDONLY);
    if(fd < 0){
        printf("open gpio%d failed\n",gpio);
        return -1;
    }
    read(fd,&tmp_value,1);
    if(tmp_value == '1'){
        *value = 1;
    }else if(tmp_value == '0'){
        *value = 0;
    }
    close(fd);
    return 0;
}

int gpio_set_edge(unsigned int gpio,unsigned int edge_flag)
{
    int fd,len;
    char buf[GPIO_MAX_BUF] = "";

    len = snprintf(buf,sizeof(buf),GPIO_SYS_PATH "/gpio%d/edge",gpio);
    if(len < 0){
        printf("write gpio%d to buf failed\n",gpio);
        return -1;
    }

    fd = open(buf,O_WRONLY);
    if(fd < 0){
        printf("open gpio%d failed\n",gpio);
        return -1;
    }

    switch(edge_flag){
        case EDGE_NONE:
            if((write(fd,"none",5)) < 0)
            {
                printf("write \"none\" failed\n");
            }
            break;
        case EDGE_RISING:
            if((write(fd,"rising",7)) < 0){
                printf("write \"rising\" failed\n");
            }
            break;
        case EDGE_FALLING:
            if((write(fd,"falling",8)) < 0){
                printf("write \"falling\" failed\n");
            }
            break;
        case EDGE_BOTH:
            if((write(fd,"both",5)) < 0){
                printf("write \"both\" failed\n");
            }
            break;
    }
    close(fd);
    return 0;
}
