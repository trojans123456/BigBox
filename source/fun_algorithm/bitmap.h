#ifndef __BITMAP_H
#define __BITMAP_H

/*bitmap 解决大数据的排序 查询
* 缺点:所有数据不能重复。即不可对重复的数据进行排序和查找*/

#define BITMAP_UINT_SIZE    32U
/* 0x1f = 0001 1111 = 31 */
#define BITMAP_UINT_MASK    0x0000001F
/* 1 << 5 or 1>> 5 = 31 or 0 */
#define BITMAP_UINT_BITS    5U

#define BITMAP_MASK(nr) ((1UL << (BITMAP_UNIT_SIZE - 1U - ((nr) & BITMAP_UNIT_MASK)))
#define BITMAP_WORD(nr) ((nr) >> BITMAP_UNIT_BITS)

/* set 设置所在位 为1
* a[i / 32] |= (1 << (i % 32)
* a[i >> 5] |= (1 << (i & 0x1f)
*/
void set(int i)
{
    bitmap[i >> BITMAP_UINT_BITS] |= (1 << (i & BITMAP_UINT_MASK));
}
/* 清零 */
void clr(int i)
{
    bitmap[i >> BITMAP_UINT_BITS] &= ~(1 << (i & BITMAP_UINT_MASK));
}

/* test 所在位是否为 1*/
int test(int i)
{
    return bitmap[i >> BITMAP_UINT_BITS] & (1 << (i & BITMAP_UINT_MASK));
}

/*
 i & 0xf1 相当于 i % 32
*/

/**
 ** This MACRO will declare a bitmap
 ** @param[in]  name  the name of the bitmap to declare
 ** @param[in]  bits  the bits of the bitmap
 ** @return  no return
 **/
#define BITMAP_DECLARE(name, bits) uint32_t name[((bits) + (BITMAP_UNIT_SIZE - 1U)) >> BITMAP_UNIT_BITS]

/**
 ** This function will set a bit of the bitmap
 ** @param[in]  bitmap  pointer to the bitmap
 ** @param[in]  nr      position of the bitmap to set
 ** @return  no return
 **/
inline void bitmap_set(uint32_t *bitmap, int32_t nr)
{
    bitmap[BITMAP_WORD(nr)] |= BITMAP_MASK(nr);
}

/**
 ** This function will clear a bit of the bitmap
 ** @param[in]  bitmap  pointer to the bitmap
 ** @param[in]  nr      position of the bitmap to clear
 ** @return  no return
 **/
inline void bitmap_clear(uint32_t *bitmap, int32_t nr)
{
    bitmap[BITMAP_WORD(nr)] &= ~BITMAP_MASK(nr);
}

/* Count Leading Zeros (clz)
   counts the number of zero bits preceding the most significant one bit. */
inline uint8_t bitmap_clz32(uint32_t x)
{
    uint8_t n = 0;

    if (x == 0) {
        return 32;
    }


    if ((x & 0XFFFF0000) == 0) {
        x <<= 16;
        n += 16;
    }
    if ((x & 0XFF000000) == 0) {
        x <<= 8;
        n += 8;
    }
    if ((x & 0XF0000000) == 0) {
        x <<= 4;
        n += 4;
    }
    if ((x & 0XC0000000) == 0) {
        x <<= 2;
        n += 2;
    }
    if ((x & 0X80000000) == 0) {
        n += 1;
    }


    return n;
}

/* Count Trailing Zeros (ctz)
   counts the number of zero bits succeeding the least significant one bit. */
inline uint8_t bitmap_ctz32(uint32_t x)
{
    uint8_t n = 0;

    if (x == 0) {
        return 32;
    }

    if ((x & 0X0000FFFF) == 0) {
        x >>= 16;
        n += 16;
    }
    if ((x & 0X000000FF) == 0) {
        x >>= 8;
        n += 8;
    }
    if ((x & 0X0000000F) == 0) {
        x >>= 4;
        n += 4;
    }
    if ((x & 0X00000003) == 0) {
        x >>= 2;
        n += 2;
    }
    if ((x & 0X00000001) == 0) {
        n += 1;
    }

    return n;
}


/**
 ** This function will find the first bit(1) of the bitmap
 ** @param[in]  bitmap  pointer to the bitmap
 ** @return  the first bit position
 **/
inline int32_t bitmap_find_first_bit(uint32_t *bitmap)
{
    int32_t  nr  = 0;
    uint32_t tmp = 0;

    while (*bitmap == 0UL) {
        nr += BITMAP_UNIT_SIZE;
        bitmap++;
    }

    tmp = *bitmap;

    if (!(tmp & 0XFFFF0000)) {
        tmp <<= 16;
        nr   += 16;
    }

    if (!(tmp & 0XFF000000)) {
        tmp <<= 8;
        nr   += 8;
    }

    if (!(tmp & 0XF0000000)) {
        tmp <<= 4;
        nr   += 4;
    }

    if (!(tmp & 0XC0000000)) {
        tmp <<= 2;
        nr   += 2;
    }

    if (!(tmp & 0X80000000)) {
        nr   += 1;
    }


    return nr;
}

#endif
