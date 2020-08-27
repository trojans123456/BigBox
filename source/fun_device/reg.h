#ifndef E_REG_H_
#define E_REG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

//某一位置1
inline void set_bit(char c,int bit)
{
    if(bit >= 0 && bit < 8)
    {
        char t = (0x01 << bit);
        t = (t ^ 0xff);
        c &= t;
    }
}
//某一位置0
inline void reset_bit(char c,int bit)
{
    if(bit >= 0 && bit < 8)
    {
        char t = (0x01 << bit);
        t = (t ^ 0xff);
        c &= t;
    }
}

inline int is_bit0(char c,int bit)
{
    c = (c >> bit) & 0x01;
    c = c ^ 0x01;
    return (int)c;
}

inline int is_bit1(char c,int bit)
{
    c = (c >> bit) & 0x01;
    return (int)c;
}

/*读寄存器*/
#define reg8_read(addr)       *((volatile uint8_t *)(addr))
#define reg16_read(addr)      *((volatile uint16_t *)(addr))
#define reg32_read(addr)      *((volatile uint32_t *)(addr))

/*写寄存器*/
#define reg8_write(addr,val)  *((volatile uint8_t *)(addr)) = (val)
#define reg16_write(addr,val) *((volatile uint16_t *)(addr)) = (val)
#define reg32_write(addr,val) *((volatile uint32_t *)(addr)) = (val)

/*读内存*/
#define mem8_read(addr)       *((volatile uint8_t *)(addr))
#define mem16_read(addr)      *((volatile uint16_t *)(addr))
#define mem32_read(addr)      *((volatile uint32_t *)(addr))

/*写内存*/
#define mem8_write(addr,val)  *((volatile uint8_t *)(addr)) = (val)
#define mem16_write(addr,val) *((volatile uint16_t *)(addr)) = (val)
#define mem32_write(addr,val) *((volatile uint32_t *)(addr)) = (val)

/*设置/清除*/
#define  reg8setbit(addr,bitpos) \
         reg8_write((addr),(reg8_read((addr)) | (1<<(bitpos))))

#define  reg16setbit(addr,bitpos) \
         reg16_write((addr),(reg16_read((addr)) | (1<<(bitpos))))

#define  reg32setbit(addr,bitpos) \
         reg32_write((addr),(reg32_read((addr)) | (1<<(bitpos))))

#define  reg8clrbit(addr,bitpos) \
         reg8_write((addr),(reg8_read((addr)) & (0xFF ^ (1<<(bitpos)))))

#define  reg16clrbit(addr,bitpos) \
         reg16_write((addr),(reg16_read((addr)) & (0xFFFF ^ (1<<(bitpos)))))

#define  reg32clrbit(addr,bitpos) \
         reg32_write((addr),(reg32_read((addr)) & (0xFFFFFFFF ^ (1<<(bitpos)))))

/*对特定的位写操作
* data = 0x1 => 01
* mask(哪几位) = 0x3 => 11 对后面两位置位 01*/
#define reg8_write_mask(addr, data, mask) \
        reg8_write((addr),((reg8_read(addr) & (~mask)) | (mask & data)))

#define reg16_write_mask(addr, data, mask) \
        reg16_write((addr),((reg16_read(addr) & (~mask)) | (mask & data)))

#define reg32_write_mask(addr, data, mask) \
        reg32_write((addr),((reg32_read(addr) & (~mask)) | (mask & data)))

#ifdef __cplusplus
}
#endif

#endif // E_REG_H_
