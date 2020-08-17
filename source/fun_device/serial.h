#ifndef __SERIAL_GENERIAL_H_
#define __SERIAL_GENERIAL_H_

#ifdef __cplusplus
extern "C"{
#endif


#define COM_DEV_NAME_STRING_LEN 30
enum com_baud_e
{
    COM_BAUD_9600,
    COM_BAUD_19200,
    COM_BAUD_115200,
    COM_BAUD_MAX
};

enum com_data_e
{
    COM_DATA_5,
    COM_DATA_6,
    COM_DATA_7,
    COM_DATA_8,
    COM_DATA_MAX
};

enum com_parity_e
{
    COM_PARITY_NONE,
    COM_PARITY_ODD,
    COM_PARITY_EVEN,
    COM_PARITY_MAX
};

enum com_stop_e
{
    COM_STOP_1,
    COM_STOP_2,
    COM_STOP_MAX
};

int serial_open(const char *name);
int serial_close(int fd);
int serial_flush(int fd);
int serial_read(int fd,void *buf,unsigned int buflen);
int serial_read_timeout(int fd,void *buf,unsigned int buflen,unsigned int timeout);
int serial_write(int fd,const void *buf,unsigned int buflen);
int serial_write_timeout(int fd,const void *buf,unsigned int buflen,unsigned int timeout);
int serial_set_baud(int fd,unsigned int baud);
int serial_get_baud(int fd,unsigned int *baud);
int serial_set_data(int fd,unsigned int data);
int serial_get_data(int fd,unsigned int *data);
int serial_set_stop(int fd,unsigned int stop);
int serial_get_stop(int fd,unsigned int *stop);
int serial_set_parity(int fd,unsigned int parity);
int serial_get_parity(int fd,unsigned int *parity);

#ifdef __cplusplus
}
#endif


#endif // E_SERIAL_GENERIAL_H_
