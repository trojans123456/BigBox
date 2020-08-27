
#include <string.h>
#include <stdio.h>


#include "mqtt.h"



//删除字符串所有空格
void DelSpaceFromString(char *str)
{
    int i = 0;
    int j = 0;
    while('\0' != str[i])
    {
        if(' ' == str[i])
        {
            for(j = i ; j < strlen(str) ; j ++)
            {
                str[j] = str[j + 1];
            }
            continue;
        }
        i ++;
    }

    str[i] = '\0';
}

unsigned char arrayToStr(unsigned char *buf, unsigned int buflen, unsigned char *out)
{
    unsigned char strBuf[33] = {0};
    unsigned char pbuf[32];
    unsigned int i;

    for (i = 0; i < buflen; i++)
    {
        sprintf((char*)pbuf, "%02X", buf[i]);
        strncat((char*)strBuf, (char*)pbuf, 2);
    }
    strncpy((char*)out, (char*)strBuf, buflen * 2);
    out[buflen*2] = 0;
    return (buflen * 2);
}

//字节流转换为十六进制字符串
void ByteToHexStr(const unsigned char* source, char* dest, int sourceLen)
{
    short i;
    unsigned char highByte, lowByte;

    for (i = 0; i < sourceLen; i++)
    {
        highByte = source[i] >> 4;
        lowByte = source[i] & 0x0f ;

        highByte += 0x30;

        if (highByte > 0x39)
                dest[i * 2] = highByte + 0x07;
        else
                dest[i * 2] = highByte;

        lowByte += 0x30;
        if (lowByte > 0x39)
            dest[i * 2 + 1] = lowByte + 0x07;
        else
            dest[i * 2 + 1] = lowByte;
    }
    return ;
}

//十六进制字符串转换为字节流
void HexStrToByte(const char* source, unsigned char* dest, int sourceLen)
{
    short i;
    unsigned char highByte, lowByte;

    for (i = 0; i < sourceLen; i += 2)
    {
        highByte = toupper(source[i]);
        lowByte  = toupper(source[i + 1]);

        if (highByte > 0x39)
            highByte -= 0x37;
        else
            highByte -= 0x30;

        if (lowByte > 0x39)
            lowByte -= 0x37;
        else
            lowByte -= 0x30;
        //dest[i / 2]
        dest[i >> 1] = (highByte << 4) | lowByte;
    }
    return ;
}

int mqtt_test_main(int argc,char *argv[])
{
    //char buffer[1024] = "10 D7 01 00 04 4D 51 54 54 04 C2 00 14 00 09 54 45 53 54 30 30 30 38 35 00 12 39 31 31 31 30 31 31 33 4D 41 30 31 43 46 38 46 38 33 00 AC 4A 76 4C 38 73 6F 39 36 7A 79 4D 36 70 70 61 54 50 66 45 65 32 4A 52 74 39 6C 73 6E 4A 30 37 45 68 54 2F 6F 51 68 63 43 41 79 75 45 37 45 79 6F 35 52 6F 51 30 4D 58 42 49 58 79 79 44 31 33 63 4E 4E 32 4C 71 4B 33 56 69 48 4C 4B 43 46 62 45 2F 49 6B 4B 58 70 65 44 66 49 4D 70 43 57 74 38 6E 69 56 6E 32 39 56 70 61 66 33 38 67 74 56 66 30 6E 65 37 52 57 50 70 48 43 34 50 6C 50 2B 67 49 57 4C 50 52 56 55 56 31 65 69 31 52 53 65 43 57 66 4A 34 47 74 44 4A 30 66 75 4F 75 71 37 69 6A 30 67 71 2F 34 42 49 69 4B 55 3D";
    unsigned char buffer[1024] = "";
    unsigned char value[1024];
    struct mqtt_response response = {0};

    #if 0
    mqtt_pack_connection_request(value,1024,"this",NULL,NULL,0,"ht","xxx",1,1);

    ByteToHexStr(value,buffer, 1024);
    printf("-- %s \n",buffer);

    DelSpaceFromString(buffer);




    memset(value,0x00,sizeof(value));

    HexStrToByte(buffer,value,strlen(buffer));


    mqtt_unpack_response(&response,value,sizeof(value));



    printf("--%d ",response.fixed_header.remaining_length);
    printf("--%d ",response.decoded.connect.protocol_name.name_len);
    int i = 0;
    for(i;i < 4; i++)
    {
        printf("--%c  ",response.decoded.connect.protocol_name.protocol_name[i]);
    }

    printf("---%d \n",response.decoded.connect.protocol_level);

    char client_id[32] = "";
    sprintf(client_id,"%s",response.decoded.connect.client_id.str);

    printf("%s \n",client_id);

#endif
    printf("---------------------------------\n");
    memset(buffer,0x00,sizeof(buffer));
    mqtt_pack_subscribe_request(buffer,1024,1,"topic_1",1,"topic_2,",2,NULL);

    mqtt_unpack_response(&response,buffer,sizeof(buffer));
}
