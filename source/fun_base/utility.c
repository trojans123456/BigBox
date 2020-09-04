#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "utility.h"

bool is_little_endian(void)
{
    static uint16_t x = 0x01;
    return *((uint8_t *) &x);
}


/*
*@brief 在大端中，将主机字节序转换为小端字节序
*@params
*   value[in] 主机字节序的值  16位
*@return 返回小端字节序  (小端中什么都不干)
*/
uint16_t cpu_to_le16(const uint16_t value)
{
    union
    {
        uint8_t b8[2];
        uint16_t b16;
    }_tmp;

    _tmp.b8[1] = (uint8_t) (value >> 8);
    _tmp.b8[0] = (uint8_t) (value & 0xff);

    return _tmp.b16;
}

uint16_t le16_to_cpu(const uint16_t value)
{
    return cpu_to_le16(value); /*或者用宏 #define le16_to_cpu(value) cpu_to_le16(value)*/
}

/*
*@breif 32位字节序转换
*/
uint32_t cpu_to_le32(const uint32_t value)
{
    return (value & 0x000000FFU) << 24 | (value & 0x0000FF00U) << 8 |
        (value & 0x00FF0000U) >> 8 | (value & 0xFF000000U) >> 24;
}

/*
*@brief 64位转换
*/
uint64_t cpu_to_le64(const uint64_t value)
{
    uint32_t high = (uint64_t)cpu_to_le32((uint32_t)value); /*低32位转换为小端*/
    uint64_t low = (uint64_t)cpu_to_le32((uint32_t)(value >> 32)); /*高32位转换为大端*/

    return ((uint64_t)high << 32) + low;
}

uint8_t bin2bcd_8(uint32_t val)
{
    return ((val / 10) << 4) + val % 10;
}

uint32_t bcd2bin_8(uint8_t val)
{
    return (val & 0x0f) + (val >> 4) * 10;
}

int bin2bcd(uint8_t *bcd,uint64_t bin,int bcd_len)
{
     int i;
    unsigned char buf[16] = {0};

    if(bcd_len > 16)
        return -1;

    //BIN switch to DEC, and save to buf for little endian
    for (i = 0; i<bcd_len; i++)
    {
        buf[i] = bin % 100;
        bin /= 100;
    }

    //DEC switch to BCD, and ouput to bcd for little endian
    for (i = 0; i<bcd_len; i++)
    {
        bcd[i]  = buf[i] % 10;
        bcd[i] |= (buf[i] / 10)<<4;
    }

    return 0;
}

static const char base64_table[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
int base64_encode(char*in,unsigned in_len,char*out){
    register unsigned X1,X2,X3;
    char*in_end=in+in_len;
    if(in_len<1)return -1;
    for(;in<in_end;in++){//for(;in<in_end;in++){
        X1=(in>in_end)?(0):(*(in++));
        X2=(in>in_end)?(0):(*(in++));
        X3=(in>in_end)?(0):(*in);
        *(out++)=base64_table[(X1&252)>>2];//*(out++)=base64_table[(X1&252)/4];
        *(out++)=base64_table[(((X2&240)>>4)|((X1&3)<<4))&255];//*(out++)=base64_table[(((X2&240)/16)|((X1&3)*16))&255];
        *(out++)=base64_table[(((X3&192)>>6)|((X2&15)<<2))&255];//*(out++)=base64_table[(((X3&192)/64)|((X2&15)*4))&255];
        *(out++)=base64_table[X3&63];
    }
    *out=0;
    unsigned len_buf=in_len%3;//in_len%3
    if(len_buf>0){
        *(--out)='=';
        if(len_buf==1)*(--out)='=';
    }
    return 1;
}

int base64_decode(char*in,unsigned in_len,char*out){
    //static const char base64_table[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    //register unsigned char len_buf=in_len&3;//in_len%4;
    register unsigned char i,s,len_buf=in_len&3;
    if(in_len<1 || len_buf>0)return -1;
    if(in[in_len-1]=='='){
        len_buf=3;
        in[in_len-1]='A';
        if(in[in_len-2]=='='){
            len_buf=2;
            in[in_len-2]='A';
        }
    }
    //char*in_end=in+in_len;
    register int N1,N2,N3,N4;
    for(;*in;in++){//for(;in<in_end;in++){
        //N1=base64_loop(*(in++));
        s=*(in++);i=63;for(;i--;)if(s==base64_table[i]){N1=i;break;}
        //N2=base64_loop(*(in++));
        s=*(in++);i=63;for(;i--;)if(s==base64_table[i]){N2=i;break;}
        //N3=base64_loop(*(in++));
        s=*(in++);i=63;for(;i--;)if(s==base64_table[i]){N3=i;break;}
        //N4=base64_loop(*in);
        s=*in;i=63;for(;i--;)if(s==base64_table[i]){N4=i;break;}
        *(out++)=(((N2&48)>>4|N1<<2)&255);//*(out++)=(((N2&48)/16|N1*4)&255);
        *(out++)=(((N3&60)>>2|N2<<4)&255);//*(out++)=(((N3&60)/4|N2*16)&255);
        *(out++)=(((N3&3)*64&255)|(N4&63));
    }
    *out=0;
    return 1;
}

unsigned long long _pow(int x,int y)
{
    unsigned long long r = 1;
    while(y--)
    {
        r *= x;
    }
    return r;
}

/*十六进制转10进制*/
unsigned long long hex2dec(char *_16)
{
    #if 0
  //printf(">%s\n",_10to16(1240568749));
    //10进制值<<从右0至左的位置编号*4
    //printf(">>>>>%d\n",(11<<12));
    //char*_16="33";
    int slen=strlen(_16);
    int i=slen;
    //printf("i:%d\n",i);
    uint64_t hex=0;
    //printf("size: %d\n",sizeof(hex));
    //uint64_t ul=strtoul(_16,0,16);
    //printf ("strtoul: %lu\n", ul);
    while(i--){
        register uint8_t Z=_16[i];
        register uint8_t N=Z;
        //printf("%d -----> %c\n",i,Z);
        //printf("%ld    %ld    %ld\n",i,(slen-i-1),(slen-i-1)<<2);
        if(Z>='A'&&Z<='F'){
            //pow (7.0, 3.0)==7 ^ 3
            //hex+=((Z-55)<<((slen-i-1)<<2));
            //hex+=(Z-55)*_pow(16,(slen-i-1));
            N=55;
        }else if(Z>='a'&&Z<='f'){
            //hex+=((Z-87)<<((slen-i-1)<<2));
            //hex+=(Z-87)*_pow(16,(slen-i-1));
            N=87;
        }else if(Z>='0'&&Z<='9'){
            //hex+=((Z-48)<<((slen-i-1)<<2));
            //hex+=(Z-48)*_pow(16,(slen-i-1));
            N=48;
        }
        hex+=(Z-N)*_pow(16,(slen-i-1));
        //printf(">>> %I64d     %u\n",hex,_pow(16,(slen-i-1)));
        //printf(">>> %llu\n",hex);
    }
    //printf("-----------\n");
    return hex;
    #endif
    int dwhexnum=0;
    char *phex = _16;
    if ((phex[0] == '0') && (phex[1] == 'x' || phex[1] == 'X')) {
        phex = phex + 2;
    }
    for (; *phex!=0 ; phex++) {
         dwhexnum *= 16;
         if ((*phex>='0') && (*phex<='9'))
            dwhexnum += *phex-'0';
         else if ((*phex>='a') && (*phex<='f'))
            dwhexnum += *phex-'a'+10;
         else if ((*phex>='A') && (*phex<='F'))
            dwhexnum += *phex-'A'+10;
         else {
            printf("hex format error!\n");
            exit(0);
         }
    }
     return dwhexnum;
}

/*10转16进制*/
int dec2hex(unsigned long long value,char *output)
{
    static const char table[]="0123456789abcdef";
    char tmp[12]={0};
    char*P=(char*)&tmp;
    char*P_save=P;
    P+=10;
    do{
        *(P--)=table[value&15];//%16
        if(P<P_save)break;
    }while((value=(value>>4))!=0);//value/=16
    ++P;
    strcpy(output,P);//memcpy(out,P,(P_save+9-P+1));
    return strlen(tmp);
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


typedef union Test
{
    int i;
    char c;
}Test;

int check_sys()
{
    Test test = { 0 };
    test.i = 1;
    if (test.c == 1)
    {
        printf("litte \n");
        return 1;
    }
    else
    {
        printf("big \n");
        return 0;
    }
}

const static char* const ascii_table = "0123456789ABCDEF";


int hex_to_ascii(const uint8_t* in_data, uint32_t in_len, uint8_t* out_buf, uint32_t* out_len)
{
    int32_t i;
    uint8_t v;
    uint8_t high, low;

    for (i = in_len - 1; i >= 0; i--)
    {
        v = in_data[i];
        high = (v >> 4) & 0x0F;
        low = (v & 0x0F);

        out_buf[(i << 1)] = ascii_table[high];
        out_buf[(i << 1) + 1] = ascii_table[low];
    }
    *out_len = in_len << 1;
    return 0;
}


int ascii_to_hex(const uint8_t* in_data, uint32_t in_len, uint8_t* out_buf, uint32_t* out_len)
{
    uint32_t i = 0;
    uint32_t j = 0;
    int h;
    int l;
    if ((in_data == NULL) || (out_buf == NULL) || (in_len % 2) != 0)
    {
        return -1;
    }
    if ((in_len >> 1) > *out_len)
    {
        return -1;
    }
    while (i < in_len)
    {
        h = char_to_hex(in_data[i]);
        if (h < 0)
        {
            return -1;
        }
        l = char_to_hex(in_data[i + 1]);
        if (l < 0)
        {
            return -1;
        }
        out_buf[j++] = (uint8_t)((h << 4) | (l & 0x0F));
        i += 2;
    }
    *out_len = j;
    return 0;
}
