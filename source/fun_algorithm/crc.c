#include "crc.h"

uint8_t crc8(uint8_t *ptr,int len)
{
    unsigned char crc;
    unsigned char i,data;
    crc = 0;
    while(len--)
    {
        data = (*ptr)&0xFF;
        crc ^= data;
        for(i = 0; i < 8; i++)
        {
            if(crc & 0x01)
            {
                crc = (crc >> 1) ^ 0x8C;
            }
            else
                crc >>= 1;
        }
        ptr++;
    }
    return crc;
}

uint16_t crc16(uint8_t *input,int len)
{
    int i,j;

    unsigned short crc = (unsigned short)0xFFFF;

    for(i=0; i<len; i++)
    {
        crc ^= (unsigned short)(input[i]);

        for(j=0; j<8; j++)
        {
            if(crc & 1)
            {
                crc >>= 1;
                crc ^= 0xA001;
            }
            else
            {
                crc >>= 1;
            }
        }
    }

    return crc;
}

uint32_t crc32(uint8_t *input,int len)
{
    int i,j;
    unsigned long r;

    r = 0xFFFFFFFFUL;
    for(i = 0; i < len; i++)
    {
        r ^= input[i];
        printf("[%d  ]: 0x%lx\n",i, r);
        for (j=0; j<8; j++)
        {
            if(r & 1)
                r = (r >> 1) ^ 0xEDB88320;
            else
                r >>= 1;

            printf("[%d-%d]: 0x%lx\n",i,j, r);
        }
        printf("[%d-f]: 0x%lx\n",i, r);
    }
    return r ^ 0xFFFFFFFFUL;
}
