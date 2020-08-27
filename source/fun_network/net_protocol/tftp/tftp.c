#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "tftp.h"

int request_package(int opcode,char *filename, int mode, char *buffer)
{
    if(NULL == buffer || NULL == filename)
        return -1;

    int cur_size = 0;
    int len = 0;

    /* opcode */
    buffer[0] = 0; //\0 = 0 = 30 ascii
    if(is_read_request(opcode))
    {
        buffer[1] = TFTP_CMD_RRQ;
    }
    else if(is_write_request(opcode))
    {
        buffer[1] = TFTP_CMD_WRQ;
    }

    int i = 0;
    len = strlen(filename);
    for(i = 0;i < len;i++)
    {
        buffer[2 + i] = filename[i];
    }

    cur_size = len + 2;
    buffer[cur_size] = 0;
    cur_size++;

    if(is_octet(mode))
    {
        len = strlen(TFTP_XFER_OCTET);
        memcpy(&buffer[cur_size],TFTP_XFER_OCTET,len);
        cur_size += len;
    }
    else if(is_netascii(mode))
    {
        len = strlen(TFTP_XFER_ASCII);
        memcpy(&buffer[cur_size],TFTP_XFER_ASCII,len);
        cur_size += len;
    }

    buffer[cur_size++] = '\0';

#if 1
    int pos = 0;
    for(pos = 0;pos < cur_size;pos++)
    {
        printf("%x ",buffer[pos]);
    }
#endif // 0

    return 1;
}

int error_package(int err_type,char *err_message,char *buffer)
{
    if((err_type <= TFTP_ERR_NOT_DEFINED) || (err_type >= TFTP_ERR_NO_USER))
    {
        return 0;
    }
    int len = strlen(err_message);
    if(len >= 512)
    {
        return 0;
    }

    buffer[0] = 0;
    buffer[1] = TFTP_CMD_ERROR;

    buffer[2] = 0;
    buffer[3] = (char)err_type;

    memcpy(&buffer[4],err_message,len);

    return 1;
}

int find_error()
{
    int tmp_error = errno;
    switch(tmp_error)
    {
    case EACCES:
        return TFTP_ERR_ACCESS_VIOLATION;
    case ENOMEM:
        return TFTP_ERR_DISK_FULL;
    case ENOSPC:
        return TFTP_ERR_FILE_NOT_FOUND;
    default:
        return TFTP_ERR_NOT_DEFINED;
    }
}

int create_error_packet_from_raw(char *buffer,int blen,packet *pkt)
{
    if(blen < 3)
        return 0;

    pkt->error_code = (int)buffer[1];
    if(pkt->err_message)
    {
        strcpy(pkt->err_message,buffer + 2);
    }
    return 1;
}

int data_package(int block_number,char *data_buffer, int data_size, char *buffer)
{
    if(NULL == data_buffer || NULL == buffer)
        return 0;

    buffer[0] = 0;
    buffer[1] = TFTP_CMD_DATA;

    buffer[2] = (char)(block_number / 256);
    buffer[3] = (char)(block_number % 256);

    memcpy(&buffer[4],data_buffer,data_size);

    return 1;
}

int create_data_packet_from_raw(char *buffer, int blen, packet* pckt )
{
    int n1,n2,i;
    char *data = NULL;
    if(blen >= 2)
    {
        n1 = (int)buffer[0];
        n2 = (int)buffer[1];

        if(n1 < 0)
        {
            n1 = 256 + n1;
        }
        if(n2 < 0)
        {
            n2 = 256 + n2;
        }

        pckt->block_number = 256 * n1 + n2;
        memcpy(pckt->data_buffer,&buffer[2],blen - 2);

        pckt->data_buffer[blen - 2] = '\0';
        return 1;
    }

    return 0;
}

int ack_package(int block_num, char *buffer)
{
    if(NULL == buffer)
        return 0;

    buffer[0] = 0;
    buffer[1] = TFTP_CMD_ACK;

    buffer[2] = (char)(block_num / 256);
    buffer[3] = (char)(block_num % 256);

    return 1;
}

int create_ack_packet_from_raw(char *buffer, int blen, packet *pckt)
{
    int n1,n2;
    if(blen != 2)
        return 0;

    n1 = (int)buffer[0];
    n2 = (int)buffer[1];

    if(n1 < 0)
    {
        n1 = 256 + n1;
    }
    if(n2 < 0)
    {
        n2 = 256 + n2;
    }

    pckt->block_number = 256 * n1 + n2;

    return 1;
}

int parser_raw_packet(char *buffer,int blen,packet *pckt)
{
    int opcode;
    if(NULL == pckt)
        return 0;

    if(buffer[0] != 0)
    {
        printf("wrong format\n");
        return 0;
    }
    opcode = (int)buffer[1];
    switch(opcode)
    {
    case TFTP_CMD_RRQ:
        printf("i am client\n");
        break;
    case TFTP_CMD_WRQ:
        printf("i am client\n");
        break;
    case TFTP_CMD_DATA:
        pckt->opcode = TFTP_CMD_DATA;
        return create_data_packet_from_raw(buffer + 2, blen - 2,pckt);//buffer+2 È¥µô²Ù×÷Âë
    case TFTP_CMD_ACK:
        pckt->opcode = TFTP_CMD_ACK;
        return create_ack_packet_from_raw(buffer + 2,blen - 2,pckt);
    case TFTP_CMD_ERROR:
        pckt->opcode = TFTP_CMD_ERROR;
        return create_error_packet_from_raw(buffer + 2,blen - 2,pckt);
    default:
        printf("not found tftp cmd\n");
        return 0;
    }
}

int init_packet(packet *pkg,char *filename,char *err_message,char *data_buffer)
{
    if(NULL == pkg)
        return 0;

    pkg->opcode = -1;
    pkg->mode = -1;
    pkg->block_number = -1;
    pkg->error_code = -1;

    if(filename)
    {
        pkg->filename = filename;
    }
    else
    {
        //pkg->filename = malloc;
    }

    if(err_message)
    {
        pkg->err_message = err_message;
    }
    else
    {
        //malloc
    }

    if(data_buffer)
    {
        pkg->data_buffer = data_buffer;
    }
    else
    {
        //malloc
    }

    return 1;
}
