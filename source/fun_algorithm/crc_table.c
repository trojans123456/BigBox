#include <stdio.h>

#include "crc_table.h"

#define CRC_POLY_16         0xA001
#define CRC_POLY_32         0xEDB88320ul
#define CRC_POLY_64         0x42F0E1EBA9EA3693ull

static uint8_t sht75_crc_table[] = {

    0,   49,  98,  83,  196, 245, 166, 151, 185, 136, 219, 234, 125, 76,  31,  46,
    67,  114, 33,  16,  135, 182, 229, 212, 250, 203, 152, 169, 62,  15,  92,  109,
    134, 183, 228, 213, 66,  115, 32,  17,  63,  14,  93,  108, 251, 202, 153, 168,
    197, 244, 167, 150, 1,   48,  99,  82,  124, 77,  30,  47,  184, 137, 218, 235,
    61,  12,  95,  110, 249, 200, 155, 170, 132, 181, 230, 215, 64,  113, 34,  19,
    126, 79,  28,  45,  186, 139, 216, 233, 199, 246, 165, 148, 3,   50,  97,  80,
    187, 138, 217, 232, 127, 78,  29,  44,  2,   51,  96,  81,  198, 247, 164, 149,
    248, 201, 154, 171, 60,  13,  94,  111, 65,  112, 35,  18,  133, 180, 231, 214,
    122, 75,  24,  41,  190, 143, 220, 237, 195, 242, 161, 144, 7,   54,  101, 84,
    57,  8,   91,  106, 253, 204, 159, 174, 128, 177, 226, 211, 68,  117, 38,  23,
    252, 205, 158, 175, 56,  9,   90,  107, 69,  116, 39,  22,  129, 176, 227, 210,
    191, 142, 221, 236, 123, 74,  25,  40,  6,   55,  100, 85,  194, 243, 160, 145,
    71,  118, 37,  20,  131, 178, 225, 208, 254, 207, 156, 173, 58,  11,  88,  105,
    4,   53,  102, 87,  192, 241, 162, 147, 189, 140, 223, 238, 121, 72,  27,  42,
    193, 240, 163, 146, 5,   52,  103, 86,  120, 73,  26,  43,  188, 141, 222, 239,
    130, 179, 224, 209, 70,  119, 36,  21,  59,  10,  89,  104, 255, 206, 157, 172
};

uint8_t crc8_tab(const uint8_t *input_str,int len)
{
    int a;
    uint8_t crc;
    const uint8_t *ptr;

    crc = 0x00;
    ptr = input_str;

    if(ptr != NULL)
    {
        for(a = 0;a < len;a++)
        {
            crc = sht75_crc_table[(*ptr++) ^ crc];
        }
    }

    return crc;
}

static uint16_t crc_tab16[256];
static void init_crc16_table(void)
{
    uint16_t i;
    uint16_t j;
    uint16_t crc;
    uint16_t c;

    for (i=0; i<256; i++) {

        crc = 0;
        c   = i;

        for (j=0; j<8; j++) {

            if((crc ^ c) & 0x0001)
                crc = ( crc >> 1 ) ^ CRC_POLY_16;
            else
                crc =   crc >> 1;

            c = c >> 1;
        }

        crc_tab16[i] = crc;
    }
}

uint16_t crc16_tab(const uint8_t *input_str,int len)
{
    uint16_t crc;
    const unsigned char *ptr;
    size_t a;

    init_crc16_table();

    crc = 0x00;
    ptr = input_str;

    if ( ptr != NULL )
    {
        for (a=0; a<len; a++)
        {
            crc = (crc >> 8) ^ crc_tab16[ (crc ^ (uint16_t) *ptr++) & 0x00FF ];
        }
    }

    return crc;
}

static  uint32_t crc_table32[256];
static void init_crc32_table(void)
{
    uint32_t c;
    uint32_t i,j;

    for(i = 0; i < 256; i++)
    {
        c = (uint32_t)i;
        for(j = 0; j < 8; j++)
        {
            if(c & 1)
                c = CRC_POLY_32 ^ (c >> 1);
            else
                c = c >> 1;
        }
        crc_table32[i] = c;
    }
}

uint32_t crc32_tab(const uint8_t *input_str, int len)
{
    unsigned int i;
    uint32_t crc = 0;

    init_crc32_table();

    for (i = 0; i < len; i++) {
        crc = crc_table32[(crc ^ input_str[i]) & 0xff] ^ (crc >> 8);
    }
    return crc ;
}
