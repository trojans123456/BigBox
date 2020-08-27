#ifndef TFTP_H
#define TFTP_H

/*tftp的报文类型
*/
#define TFTP_CMD_RRQ    (1)     /*read request (RRQ)*/
#define TFTP_CMD_WRQ    (2)     /*write request (WRQ)*/
#define TFTP_CMD_DATA   (3)     /*data (DATA)*/
#define TFTP_CMD_ACK    (4)     /*acknowledgment(ACK)*/
#define TFTP_CMD_ERROR  (5)     /*error (ERROR)*/


#define is_read_request(opcode)     ((opcode) == TFTP_CMD_RRQ)
#define is_write_request(opcode)    ((opcode) == TFTP_CMD_WRQ)
#define is_data_packet(opcode)      ((opcode) == TFTP_CMD_DATA)
#define is_error_packet(opcode)     ((opcode) == TFTP_CMD_ERROR)
#define is_ack_packet(opcode)       ((opcode) == TFTP_CMD_ACK)

/*tftp的传输类型
*/
#define TFTP_XFER_OCTET     ("octet")
#define TFTP_XFER_ASCII     ("netascii")
#define TFTP_OCTET_NUMBER   (1)
#define TFTP_ASCII_NUMBER   (2)

#define is_octet(mode)      ((mode) == TFTP_OCTET_NUMBER)
#define is_netascii(mode)      ((mode) == TFTP_ASCII_NUMBER)

/*错误类型
*/
#define TFTP_ERR_NOT_DEFINED        0
#define TFTP_ERR_FILE_NOT_FOUND     1
#define TFTP_ERR_ACCESS_VIOLATION   2
#define TFTP_ERR_DISK_FULL          3
#define TFTP_ERR_ILLEGAL_OPERATION  4
#define TFTP_ERR_UNKNOWN_TID        5
#define TFTP_ERR_FILE_EXISTS        6
#define TFTP_ERR_NO_USER            7

typedef struct
{
    int opcode;
    char *filename;
    int mode;
    int block_number;
    int error_code;
    char *err_message;
    char *data_buffer;
}packet;

/****API****/
/*
*@brief 初始化packet
*/
int init_packet(packet *pkg,char *filename,char *err_message,char *data_buffer);

/*
*@brief 处理从服务器收到的数据
*/
int parser_raw_packet(char *buffer,int blen,packet *pckt);

/*
*@brief 组装request
*/
int request_package(int opcode,char *filename, int mode, char *buffer);

/*
*@brief 组装error
*/
int error_package(int err_type,char *err_message,char *buffer);

/*
*@brief 根据socket的errno寻找错误号
*/
int find_error();

/*
*@brief 从服务端接收到的error信息 保证pkt从外部创建足够大的err_message
*/
int create_error_packet_from_raw(char *buffer,int blen,packet *pkt);

/*
*@brief 组装data
*/
int data_package(int block_number,char *data_buffer, int data_size, char *buffer);

/*
*@brief 从服务端接收到的data数据
*/
int create_data_packet_from_raw(char *buffer, int blen, packet* pckt );

/*
*@brief 组装ack
*/
int ack_package(int block_num, char *buffer);

/*
*@brief 从服务器接收到的ack
*/
int create_ack_packet_from_raw(char *buffer, int blen, packet *pckt);


#endif // TFTP_H
