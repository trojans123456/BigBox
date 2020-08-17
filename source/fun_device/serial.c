#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>

#include "serial.h"

#ifdef SERIAL_DEBUG
#define serial_debug(...) do{\
                printf(__VA_ARGS__);\
                }while(0)
#else
#define serial_debug(...)
#endif // SERIAL_DEBUG

#define get_port_state(fd,state) \
    if(tcgetattr(fd,state) < 0) {\
        return -1;\
    }

#define set_port_state(fd,state) \
    if(tcsetattr(fd,TCSANOW,state) < 0) {\
        return -1;\
    }

int serial_open(const char *name)
{
    if(!name)
        return -1;

    int fd = open(name,O_RDWR | O_NOCTTY | O_NDELAY);
    if(fd < 0)
        return -1;

    tcflush(fd,TCIOFLUSH);

    struct termios term;
    get_port_state(fd,&term);

    term.c_cflag |= (CREAD | CLOCAL);
    term.c_iflag = IGNPAR;
    term.c_oflag = 0;
    term.c_lflag = 0;
    term.c_cc[VINTR]  = _POSIX_VDISABLE;
    term.c_cc[VQUIT]  = _POSIX_VDISABLE;
    term.c_cc[VSTART] = _POSIX_VDISABLE;
    term.c_cc[VSTOP]  = _POSIX_VDISABLE;
    term.c_cc[VSUSP]  = _POSIX_VDISABLE;
    term.c_cc[VEOF]   = _POSIX_VDISABLE;
    term.c_cc[VEOL]   = _POSIX_VDISABLE;
    term.c_cc[VERASE] = _POSIX_VDISABLE;
    term.c_cc[VKILL]  = _POSIX_VDISABLE;

    set_port_state(fd,&term);

    serial_set_baud(fd,COM_BAUD_115200);
    serial_set_data(fd,COM_DATA_8);
    serial_set_parity(fd,COM_PARITY_NONE);
    serial_set_stop(fd,COM_STOP_1);

    return fd;
}
int serial_close(int fd)
{
    close(fd);
    return 1;
}
int serial_flush(int fd)
{
    int ret = -1;
    ret = tcflush(fd,TCIOFLUSH);

    return ret;
}
int serial_read(int fd,void *buf,unsigned int buflen)
{
    return read(fd,buf,buflen);
}
int serial_read_timeout(int fd,void *buf,unsigned int buflen,unsigned int timeout)
{
    if(fd < 0)
        return -1;

    int ret;
    int real;
    fd_set set;
    struct timeval tv;

    FD_ZERO(&set);
    FD_SET(fd,&set);
    tv.tv_sec = (timeout * 1000) / 1000000;
    tv.tv_usec = (timeout * 1000) % 1000000;

    ret = select(fd+1,&set,NULL,NULL,&tv);
    serial_debug("select ret = %d\n",ret);
    switch(ret)
    {
    case 1:
        real = read(fd,buf,buflen);
        serial_debug("read-timeout-real:%d \n",real);
        if(real == -1)
            return -1;
        break;
    case 0:
    default:
        return -1;
    }

    return real;
}
int serial_write(int fd,const void *buf,unsigned int buflen)
{
    return write(fd,buf,buflen);
}
int serial_write_timeout(int fd,const void *buf,unsigned int buflen,unsigned int timeout)
{
    int ret;
    fd_set set;
    int real;
    struct timeval tv;

    FD_ZERO(&set);
    FD_SET(fd,&set);
    tv.tv_sec = (timeout * 1000) / 1000000;
    tv.tv_usec = (timeout * 1000) % 1000000;

    ret = select(fd + 1,NULL,&set,NULL,&tv);
    switch(ret)
    {
    case 1:
        real = write(fd,buf,buflen);
        if(real == -1)
            return -1;
        break;
    case 0:
    default:
        return -1;
    }

    return real;
}
int serial_set_baud(int fd,unsigned int baud)
{
    struct termios term;

    get_port_state(fd,&term);

    switch(baud)
    {
    case COM_BAUD_9600:
        cfsetispeed(&term,B9600);
        cfsetospeed(&term,B9600);
        break;
    case COM_BAUD_19200:
        cfsetispeed(&term,B19200);
        cfsetospeed(&term,B19200);
        break;
    case COM_BAUD_115200:
        cfsetispeed(&term,B115200);
        cfsetospeed(&term,B115200);
        break;
    default:
        return -1;
    }

    set_port_state(fd,&term);

    return 1;
}
int serial_get_baud(int fd,unsigned int *baud)
{
    return 1;
}
int serial_set_data(int fd,unsigned int data)
{
    struct termios term;
    get_port_state(fd,&term);
    term.c_cflag &= ~CSIZE;

    switch(data)
    {
    case COM_DATA_5:
        term.c_cflag |= CS5;
        break;
    case COM_DATA_6:
        term.c_cflag |= CS6;
        break;
    case COM_DATA_7:
        term.c_cflag |= CS7;
        break;
    case COM_DATA_8:
        term.c_cflag |= CS8;
        break;
    default:
        return -1;
    }

    return 1;
}
int serial_get_data(int fd,unsigned int *data)
{
    return 1;
}
int serial_set_stop(int fd,unsigned int stop)
{
    struct termios term;
    get_port_state(fd,&term);
    term.c_cflag &= ~CSTOPB;

    switch(stop)
    {
    case COM_STOP_1:
        break;
    case COM_STOP_2:
        term.c_cflag |= CSTOPB;
        break;
    default:
        return -1;
    }

    set_port_state(fd,&term);
    return 1;
}
int serial_get_stop(int fd,unsigned int *stop)
{
    return 1;
}
int serial_set_parity(int fd,unsigned int parity)
{
    struct termios term;
    get_port_state(fd,&term);

    switch(parity)
    {
    case COM_PARITY_NONE:
        term.c_cflag &= ~PARENB;
        break;
    case COM_PARITY_ODD:
        term.c_cflag |= (PARENB | PARODD);
        break;
    case COM_PARITY_EVEN:
        term.c_cflag &= ~PARODD;
        term.c_cflag |= PARENB;
        break;
    default:
        return -1;
    }

    set_port_state(fd,&term);

    return 1;
}
int serial_get_parity(int fd,unsigned int *parity)
{
    return 1;
}
