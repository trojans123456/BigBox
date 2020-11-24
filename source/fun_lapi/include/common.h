#ifndef __COMMON_H
#define __COMMON_H

/*
type
*/
typedef unsigned char boolean;

#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define MIN(x,y) ((x) < (y) ? (x) : (y))


/**
 * 将一个字母转化为大写
*/
#define UPCASE(c) (((c)>='a' && (c) <= 'z') ? ((c) – 0×20) : (c))

/**
 * 判断字符是不是10进值的数字
*/
#define  DECCHK(c) ((c)>='0' && (c)<='9')

/**
 * 判断字符是不是16进值的数字
*/
#define HEXCHK(c) (((c) >= '0' && (c)<='9') ((c)>='A' && (c)<= 'F') \
((c)>='a' && (c)<='f'))

/**
 * 返回数组元素的个数
*/
#define ARR_SIZE(a)  (sizeof((a))/sizeof((a[0])))

/**
  IO操作
*/
#ifdef __linux__
#define inp(port) (*((volatile byte *)(port)))
#define inpw(port) (*((volatile word *)(port)))
#define inpdw(port) (*((volatile dword *)(port)))
#define outp(port,val) (*((volatile byte *)(port))=((byte)(val)))
#define outpw(port, val) (*((volatile word *)(port))=((word)(val)))
#define outpdw(port, val) (*((volatile dword *)(port))=((dword)(val)))
#endif

#ifdef __cplusplus
#define C_API   extern "C"
#else
#define C_API
#endif

#define declear_handler(name)   struct name##__ {int unused;}; typedef struct name##__ * name


#endif
