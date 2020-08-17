#ifndef __I2C_H_
#define __I2C_H_

#ifdef __cplusplus
extern "C" {
#endif

int i2cdev_open(const char *devname);
void i2cdev_close(int fd);

int i2c_transfer(int fd,struct i2c_msg *msgs,int msg_cnt);

int i2c_read_data(int fd,unsigned short saddr,unsigned short regaddr,unsigned char *readbuf,unsigned short readlen);
int i2c_read_byte(int fd,unsigned short saddr,unsigned short regaddr,unsigned char data);
int i2c_write_data(int fd,unsigned short saddr,unsigned short regaddr,unsigned char *writebuf,unsigned short writelen);
int i2c_write_byte(int fd,unsigned char saddr,unsigned short regaddr,unsigned char data);

#ifdef __cplusplus
}
#endif

#endif // E_I2C_H_
