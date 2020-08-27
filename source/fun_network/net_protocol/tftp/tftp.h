#ifndef TFTP_H
#define TFTP_H

/*tftp�ı�������
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

/*tftp�Ĵ�������
*/
#define TFTP_XFER_OCTET     ("octet")
#define TFTP_XFER_ASCII     ("netascii")
#define TFTP_OCTET_NUMBER   (1)
#define TFTP_ASCII_NUMBER   (2)

#define is_octet(mode)      ((mode) == TFTP_OCTET_NUMBER)
#define is_netascii(mode)      ((mode) == TFTP_ASCII_NUMBER)

/*��������
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
*@brief ��ʼ��packet
*/
int init_packet(packet *pkg,char *filename,char *err_message,char *data_buffer);

/*
*@brief ����ӷ������յ�������
*/
int parser_raw_packet(char *buffer,int blen,packet *pckt);

/*
*@brief ��װrequest
*/
int request_package(int opcode,char *filename, int mode, char *buffer);

/*
*@brief ��װerror
*/
int error_package(int err_type,char *err_message,char *buffer);

/*
*@brief ����socket��errnoѰ�Ҵ����
*/
int find_error();

/*
*@brief �ӷ���˽��յ���error��Ϣ ��֤pkt���ⲿ�����㹻���err_message
*/
int create_error_packet_from_raw(char *buffer,int blen,packet *pkt);

/*
*@brief ��װdata
*/
int data_package(int block_number,char *data_buffer, int data_size, char *buffer);

/*
*@brief �ӷ���˽��յ���data����
*/
int create_data_packet_from_raw(char *buffer, int blen, packet* pckt );

/*
*@brief ��װack
*/
int ack_package(int block_num, char *buffer);

/*
*@brief �ӷ��������յ���ack
*/
int create_ack_packet_from_raw(char *buffer, int blen, packet *pckt);


#endif // TFTP_H
