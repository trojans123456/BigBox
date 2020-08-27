#ifndef E_REG_H_
#define E_REG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

//ĳһλ��1
inline void set_bit(char c,int bit)
{
    if(bit >= 0 && bit < 8)
    {
        char t = (0x01 << bit);
        t = (t ^ 0xff);
        c &= t;
    }
}
//ĳһλ��0
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

/*���Ĵ���*/
#define reg8_read(addr)       *((volatile uint8_t *)(addr))
#define reg16_read(addr)      *((volatile uint16_t *)(addr))
#define reg32_read(addr)      *((volatile uint32_t *)(addr))

/*д�Ĵ���*/
#define reg8_write(addr,val)  *((volatile uint8_t *)(addr)) = (val)
#define reg16_write(addr,val) *((volatile uint16_t *)(addr)) = (val)
#define reg32_write(addr,val) *((volatile uint32_t *)(addr)) = (val)

/*���ڴ�*/
#define mem8_read(addr)       *((volatile uint8_t *)(addr))
#define mem16_read(addr)      *((volatile uint16_t *)(addr))
#define mem32_read(addr)      *((volatile uint32_t *)(addr))

/*д�ڴ�*/
#define mem8_write(addr,val)  *((volatile uint8_t *)(addr)) = (val)
#define mem16_write(addr,val) *((volatile uint16_t *)(addr)) = (val)
#define mem32_write(addr,val) *((volatile uint32_t *)(addr)) = (val)

/*����/���*/
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

/*���ض���λд����
* data = 0x1 => 01
* mask(�ļ�λ) = 0x3 => 11 �Ժ�����λ��λ 01*/
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
