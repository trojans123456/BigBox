#ifndef __STROPT_H
#define __STROPT_H

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <string>

using namespace std;

#define vmin(a,b) ((a) < (b) ? (a) : (b))
#define vmax(a,b) ((a) > (b) ? (a) : (b))

#define BSWAP_16(x) \
    (uint16_t)((((uint16_t)(x) & 0x00ff) << 8) | \
              (((uint16_t)(x) & 0xff00) >> 8) \
             )


#define BSWAP_32(x) \
    (uint32_t)((((uint32_t)(x) & 0xff000000) >> 24) | \
              (((uint32_t)(x) & 0x00ff0000) >> 8) | \
              (((uint32_t)(x) & 0x0000ff00) << 8) | \
              (((uint32_t)(x) & 0x000000ff) << 24) \
             )

namespace ns_core {


class StrOpt
{
public:
    enum Code
    {
        PathEl,  //表示路径元素
        HttpURL, //表示浏览器地址
        Html,    //表示html中使用的特殊字符
        JavaSc,  //表示javascript行结束符
        SQL,     //sql查询值
        Custom,  //指定字符
        base64,   //标准的base64(网络)
        FormatPrint, //printf等函数的格式化元素的编码/掩码
        oscdID,  //节点标识符
        Bin, //二进制数据文本编码
        Reverse, //颠倒行中字符的顺序
        ShieldSimb, //屏蔽符号 像 \n 针对real code
        ToLower //转换为小写
    };
    static string strEncode( const string &in, Code tp, const string &opt1 = "" );
    static string strDecode( const string &in, Code tp = Custom, const string &opt1 = "" );

    static string strMess( const char *fmt, ... );
    static string strMess( unsigned len, const char *fmt, ... );

    enum IntView {Dec,Oct,Hex};

    //int 转换为 str
    static string int2str( int val, IntView view = Dec );
    static string uint2str( unsigned val, IntView view = Dec );
    static string ll2str( long long val, IntView view = Dec );
    //实数(double float)转换为字符串
    static string real2str( double val, int prec = 15, char tp = 'g' );
    //实数四舍五入
    static double realRound( double val, int dig = 0, bool toint = false )
    {
        double rez = floor(val*pow(10,dig)+0.5)/pow(10,dig);
        return toint ? floor(rez+0.5) : rez;
    }
    //时间转换
    static string atime2str(time_t itm, const string &format = "" );
    static string time2str( double tm );
    //
    static string cpct2str( double cnt );

    static int64_t curTime();

    // str转换为real
    static double str2real( const string &val );

    static int str2int(const string &val)
    {
        return (int)strtol(val.c_str(),NULL,10);
    }

    static long str2long(const string &val)
    {
        return strtol(val.c_str(),NULL,10);
    }

    static unsigned long long str2ulong(const string &val)
    {
        return strtoul(val.c_str(),NULL,10);
    }

    static long long str2ll(const string &val)
    {
        return strtoll(val.c_str(),NULL,10);
    }

    static unsigned long long str2ull(const string &val)
    {
        return strtoull(val.c_str(),NULL,10);
    }

    static float str2float(const string &val)
    {
        return strtof(val.c_str(),NULL);
    }

    static double str2double(const string &val)
    {
        return strtod(val.c_str(),NULL);
    }

    static long double str2ld(const string &val)
    {
        return strtold(val.c_str(),NULL);
    }

    // 地址转换为字符串
    static string addr2str( void *addr );

    static void *str2addr( const string &str );

    //> 去除字符串前后的\r\n\t
    static string strTrim( const string &val, const string &cfg = " \n\t\r" );
    //去除前后的空格，中间的字符串只保留一个空格
    static string strSimplified(const string &val);
    //字符串按等级分割 11.22.33.44 ——> 11:0 22:1 33:2 44:3 off控制偏移位置
    static string strSepParse( const string &str, int level, char sep, int *off = NULL );
    //通过sep多字符分割解析 merge是否合并
    static string strParse( const string &str, int level, const string &sep, int *off = NULL, bool mergeSepSymb = false );
    static string strLine( const string &str, int level, int *off = NULL );

    static string pathLev( const string &path, int level, bool decode = true, int *off = NULL );
    static string path2sepstr( const string &path, char sep = '.' );
    static string sepstr2path( const string &str, char sep = '.' );

    static unsigned char getBase64Code( unsigned char asymb );

    /* 新增 */
    //将buf转为16进制字串
    int b2str(const char *buf, const int bufLen, char *str, const int strLen, bool capital = true);
    //判断字串是否为纯数字
    bool strIsDigit(const char *str, const int strLen = 0);
    //判断字串是否为纯字母
    bool strIsAlpha(const char *str, const int strLen = 0);
    //判断字串是否为数字或字母
    bool strIsAlnum(const char *str, const int strLen = 0);
    //判断字串是否为大写的16进制字串（0~9 或 A~F）
    bool strIsUpperHex(const char *str, const int strLen = 0);
    //判断字串是否为 日期格式
    bool strIsDate(const char *str, const int strLen = 0);
    //将一个字符串（或2进制buf）转为uint32类型并返回
    unsigned int strToHashValue(const char *str, const int strLen = 0);


    static inline uint16_t getUnalign16( const void *p )
    {
        struct su16 { uint16_t x; } __attribute__((packed));
        const struct su16 *ptr = (const struct su16 *)p;
        return ptr->x;
    }
    static inline uint32_t getUnalign32( const void *p )
    {
        struct su32 { uint32_t x; } __attribute__((packed));
        const struct su32 *ptr = (const struct su32 *)p;
        return ptr->x;
    }
    static inline uint64_t getUnalign64( const void *p )
    {
        struct su64 { uint64_t x; } __attribute__((packed));
        const struct su64 *ptr = (const struct su64 *)p;
        return ptr->x;
    }
    static inline int getUnalignInt( const void *p )
    {
        struct suInt { int x; } __attribute__((packed));
        const struct suInt *ptr = (const struct suInt *)p;
        return ptr->x;
    }
    static inline float getUnalignFloat( const void *p )
    {
        struct sFloat64 { float x; } __attribute__((packed));
        const struct sFloat64 *ptr = (const struct sFloat64 *)p;
        return ptr->x;
    }
    static inline double getUnalignDbl( const void *p )
    {
        struct sDbl { double x; } __attribute__((packed));
        const struct sDbl *ptr = (const struct sDbl *)p;
        return ptr->x;
    }

};

}


#endif
