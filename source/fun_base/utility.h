#ifndef __UTILITY_H
#define __UTILITY_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __GNUC__

#define LIKELY(x)           (__builtin_expect(!!(x), 1))
#define UNLIKELY(x)         (__builtin_expect(!!(x), 0))

#define SWAP(a, b)          \
    do { typeof(a) __tmp = (a); (a) = (b); (b) = __tmp; } while (0)

#endif

#define MIN2(a, b)          ((a) > (b) ? (b) : (a))
#define MAX2(a, b)          ((a) > (b) ? (a) : (b))
#define ABS(x)              ((x) >= 0 ? (x) : -(x))

#define ARRAY_SIZE(a)       (sizeof(a) / sizeof(a[0]))

#define VERBOSE()                                                   \
    do {                                                            \
        printf("%s:%s:%d xxxxxx\n", __FILE__, __func__, __LINE__);  \
    } while (0)

#define ALIGN2(x, a)	(((x) + (a) - 1) & ~((a) - 1))

/**
 * @brief 是否为小端模式
 * @return
 */
bool is_littlen_endian(void);

/*字节顺序交换*/
static inline unsigned short rev2(unsigned short in)
{
    return ((in & 0x00FF) << 8) | ((in & 0xFF00) >> 8);
}

static inline unsigned int rev4(unsigned int in)
{
    return ((in & 0x000000FFU) << 24)
        |  ((in & 0x0000FF00U) << 8)
        |  ((in & 0x00FF0000U) >> 8)
        |  ((in & 0xFF000000U) >>24);
}

static inline unsigned long long rev8(unsigned long long in)
{
    return ((in & 0x00000000000000FFULL) << 56)
        |  ((in & 0x000000000000FF00ULL) << 40)
        |  ((in & 0x0000000000FF0000ULL) << 24)
        |  ((in & 0x00000000FF000000ULL) << 8)
        |  ((in & 0x000000FF00000000ULL) >> 8)
        |  ((in & 0x0000FF0000000000ULL) >> 24)
        |  ((in & 0x00FF000000000000ULL) >> 40)
        |  ((in & 0xFF00000000000000ULL) >> 56);
}

static inline void revinplc2(void *in)
{
    unsigned char tmp;
    tmp = ((unsigned char *)in)[0];
    ((unsigned char *)in)[0] = ((unsigned char *)in)[1];
    ((unsigned char *)in)[1] = tmp;
}

static inline void revinplc4(void *in)
{
    unsigned char tmp;
    tmp = ((unsigned char *)in)[0];
    ((unsigned char *)in)[0] = ((unsigned char *)in)[3];
    ((unsigned char *)in)[3] = tmp;

    tmp = ((unsigned char *)in)[1];
    ((unsigned char *)in)[1] = ((unsigned char *)in)[2];
    ((unsigned char *)in)[2] = tmp;
}

static inline void revinplc8(void *in)
{
    unsigned char tmp;
    tmp = ((unsigned char *)in)[0];
    ((unsigned char *)in)[0] = ((unsigned char *)in)[7];
    ((unsigned char *)in)[7] = tmp;

    tmp = ((unsigned char *)in)[1];
    ((unsigned char *)in)[1] = ((unsigned char *)in)[6];
    ((unsigned char *)in)[6] = tmp;

    tmp = ((unsigned char *)in)[2];
    ((unsigned char *)in)[2] = ((unsigned char *)in)[5];
    ((unsigned char *)in)[5] = tmp;

    tmp = ((unsigned char *)in)[3];
    ((unsigned char *)in)[3] = ((unsigned char *)in)[4];
    ((unsigned char *)in)[4] = tmp;
}



/* 组合16位数据*/
#define LITTE_ENDIAN    1
#ifdef LITTE_ENDIAN
#define MAKE_U16(a,b)   ((a << 8) | b)
#define MAKE_U32(a,b,c,d)   (((unsigned int)((a) & 0xff) << 24) | \
                             ((unsigned int)((b) & 0xff) << 16) | \
                             ((unsigned int)((c) & 0xff) << 8) | \
                             (unsigned int)((d) & 0xff))
#else
#define MAKE_U16(a,b)   ((b << 8) | a)
#define MAKE_U32(a,b,c,d)
#endif

typedef union
{
    unsigned short data;
    struct
    {
        unsigned char low;
        unsigned char high;
    }bytes;
}ushort_union_t;

typedef union
{
    unsigned int data;
    unsigned char data_bytes[sizeof(unsigned int)];
    struct
    {
        unsigned char byte0;
        unsigned char byte1;
        unsigned char byte2;
        unsigned char byte3;
    }bytes;
}uint_union_t;

/* if 大端 htons(x) x*/
#define htons(x)    (((x) & 0xff) << 8) | (((x) & 0xff00) >> 8))
#define ntohs(x)    htons(x)

#define htonl(x)    ((((x) & 0xff) << 24) | \
                      (((x) & 0xff00) << 8) | \
                      (((x) & 0xff0000UL) >> 8) | \
                      (((x) & 0xff000000UL) >> 24))

#define ntohl(x)    htonl(x)

/*
*@brief 在大端中，将主机字节序转换为小端字节序
*@params
*   value[in] 主机字节序的值  16位
*@return 返回小端字节序  (小端中什么都不干)
*/
uint16_t cpu_to_le16(const uint16_t value);

uint16_t le16_to_cpu(const uint16_t value);

/*
*@breif 32位字节序转换
*/
uint32_t cpu_to_le32(const uint32_t value);

/*
*@brief 64位转换
*/
uint64_t cpu_to_le64(const uint64_t value);

/*
*@brief bin和bcd转换
*/
uint8_t bin2bcd_8(uint32_t val);

uint32_t bcd2bin_8(uint8_t val);

int bin2bcd(uint8_t *bcd,uint64_t bin,int bcd_len);

/*base64 转换常用于网络传输*/
int base64_encode(char*in,unsigned in_len,char*out);

int base64_decode(char*in,unsigned in_len,char*out);

/**反转高低位*/
#define SWAP_8(x) (((x)>>4)|(((x)&0x0f)<<4))                                    //0x31 --> 0x13
#define SWAP_16(x) (((x)>>8)|(((x)&0x00ff)<<8))                                 //0x1122 --> 0x2211
#define SWAP_32_1(x) (((x)>>16)|(((x)&0x0000ffff)<<16))                         //0x11223344 --> 0x33441122
#define SWAP_32_2(x) (((x)>>24)|(((x)&0x00ff0000)>>8)|(((x)&0x0000ff00)<<8)|(((x)&0x000000ff)<<24))     //0x11223344 --> 0x44332211
#define SWAP_32 SWAP_32_2

#define ROUND(x)    ((int)((x)+0.5))    //四舍五入

/*x的y次方*/
unsigned long long _pow(int x,int y);

/*十六进制转10进制*/
unsigned long long hex2dec(char *_16);

/*10转16进制*/
int dec2hex(unsigned long long value,char *output);


//字节流转换为十六进制字符串
void ByteToHexStr(const unsigned char* source, char* dest, int sourceLen);

//十六进制字符串转换为字节流
void HexStrToByte(const char* source, unsigned char* dest, int sourceLen);

//检查大小端
int check_sys();


int hex_to_ascii(const uint8_t* in_data, uint32_t in_len, uint8_t* out_buf, uint32_t* out_len);
int ascii_to_hex(const uint8_t* in_data, uint32_t in_len, uint8_t* out_buf, uint32_t* out_len);


/**
 * @brief 随机数
 * @param seed
 */
void my_srand(unsigned int seed);
int my_rand(void);

#ifdef __cplusplus
}
#endif

#endif
