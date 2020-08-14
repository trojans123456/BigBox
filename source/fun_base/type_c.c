#include "type_c.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
unsigned long t_s2_to_u64(char const* s)
{
    // skip space
    while (t_isspace(*s)) s++;

    // has sign?
    int sign = 0;
    if (*s == '-')
    {
        sign = 1;
        s++;
    }
    // skip '+'
    else if (*s == '+') s++;

    // skip "0b"
    if (s[0] == '0' && (s[1] == 'b' || s[1] == 'B'))
        s += 2;

    // skip '0'
    while ((*s) == '0') s++;

    // compute number
    unsigned long val = 0;
    while (*s)
    {
        char ch = *s;
        if (t_isdigit2(ch))
            val = (val << 1) + (ch - '0');
        else
            break;

        s++;
    }

    // is negative number?
    if (sign) val = ~val + 1;

    // the value
    return val;
}

unsigned long t_s8_to_u64(char const* s)
{
    // skip space
    while (t_isspace(*s)) s++;

    // has sign?
    int sign = 0;
    if (*s == '-')
    {
        sign = 1;
        s++;
    }
    // skip '+'
    else if (*s == '+') s++;

    // skip '0'
    while ((*s) == '0') s++;

    // compute number
    unsigned long val = 0;
    while (*s)
    {
        char ch = *s;
        if (t_isdigit8(ch))
            val = (val << 3) + (ch - '0');
        else
            break;

        s++;
    }

    // is negative number?
    if (sign) val = ~val + 1;

    // the value
    return val;
}

unsigned long t_s10_to_u64(char const* s)
{
    // skip space
    while (t_isspace(*s)) s++;

    // has sign?
    int sign = 0;
    if (*s == '-')
    {
        sign = 1;
        s++;
    }
    // skip '+'
    else if (*s == '+') s++;

    // skip '0'
    while ((*s) == '0') s++;

    // compute number
    unsigned long val = 0;
    while (*s)
    {
        char ch = *s;
        if (t_isdigit10(ch))
            val = val * 10 + (ch - '0');
        else
            break;

        s++;
    }

    // is negative number?
    if (sign) val = ~val + 1;

    // the value
    return val;
}

unsigned long t_s16_to_u64(char const* s)
{
    // skip space
    while (t_isspace(*s)) s++;

    // has sign?
    int sign = 0;
    if (*s == '-')
    {
        sign = 1;
        s++;
    }
    // skip '+'
    else if (*s == '+') s++;

    // skip "0x"
    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
        s += 2;

    // skip '0'
    while ((*s) == '0') s++;

    // compute number
    unsigned long val = 0;
    while (*s)
    {
        char ch = *s;
        if (t_isdigit10(ch))
            val = (val << 4) + (ch - '0');
        else if (ch > ('a' - 1) && ch < ('f' + 1))
            val = (val << 4) + (ch - 'a') + 10;
        else if (ch > ('A' - 1) && ch < ('F' + 1))
            val = (val << 4) + (ch - 'A') + 10;
        else
            break;

        s++;
    }

    // is negative number?
    if (sign) val = ~val + 1;

    // the value
    return val;
}

unsigned long t_s_to_u64(char const* s)
{
    // skip space
    char const* p = s;
    while (t_isspace(*p)) p++;

    // has sign?
    if (*p == '-' || *p == '+') p++;

    // is hex?
    if (*p++ == '0')
    {
        if (*p == 'x' || *p == 'X')
            return t_s16_to_u64(s);
        else if (*p == 'b' || *p == 'B')
            return t_s2_to_u64(s);
        else return t_s8_to_u64(s);
    }
    else return t_s10_to_u64(s);
}

unsigned long t_sb_to_u64(char const* s, int base)
{
    // the convect functions
    static unsigned long (*s_conv[])(char const*) =
    {
        0
    ,   0
    ,   t_s2_to_u64
    ,   0
    ,   0
    ,   0
    ,   0
    ,   0
    ,   t_s8_to_u64
    ,   0
    ,   t_s10_to_u64
    ,   0
    ,   0
    ,   0
    ,   0
    ,   0
    ,   t_s16_to_u64
    };
    if(base > (sizeof(s_conv) / sizeof(s_conv[0])) ||
            !s_conv[base])
        return 0;

    // convect it
    return s_conv[base](s);
}


/* nan  浮点数 */
#ifdef TB_WORDS_BIGENDIAN
#define __tb_nan_bytes       { 0x7f, 0xc0, 0, 0 }
#else
#define __tb_nan_bytes       { 0, 0, 0xc0, 0x7f }
#endif
static union { unsigned char __c[4]; float __d; } __tb_nan_union = { __tb_nan_bytes };
#define TB_NAN   (__tb_nan_union.__d)

typedef union { unsigned char __c[8]; double __d; } __tb_maf_t;
#ifdef TB_WORDS_BIGENDIAN
#define __tb_maf_bytes   { 0x7f, 0xf0, 0, 0, 0, 0, 0, 0 }
#else
#define __tb_maf_bytes   { 0, 0, 0, 0, 0, 0, 0xf0, 0x7f }
#endif
static __tb_maf_t __tb_maf = { __tb_maf_bytes };
#define TB_MAF       (__tb_maf.__d)
#define TB_INF  TB_MAF

double t_s2_to_d(char const* s)
{
    // skip space
    while (t_isspace(*s)) s++;

    // has sign?
    int sign = 0;
    if (*s == '-')
    {
        sign = 1;
        s++;
    }
    // skip '+'
    else if (*s == '+') s++;

    // nan?
    if (s[0] == 'n' && s[1] == 'a' && s[2] == 'n')
        return TB_NAN;

    // inf or -inf?
    if (s[0] == 'i' && s[1] == 'n' && s[2] == 'f')
        return sign? -TB_INF : TB_INF;

    // skip "0b"
    if (s[0] == '0' && (s[1] == 'b' || s[1] == 'B'))
        s += 2;

    // compute double: lhs.rhs
    int    dec = 0;
    unsigned long lhs = 0;
    double rhs = 0.;
    int    zeros = 0;
    unsigned char   decimals[256];
    unsigned char*  d = decimals;
    unsigned char*  e = decimals + 256;

    while (*s)
    {
        char ch = *s;

        // is the part of decimal?
        if (ch == '.')
        {
            if (!dec)
            {
                dec = 1;
                s++;
                continue ;
            }
            else break;
        }

        // parse integer & decimal
        if (t_isdigit2(ch))
        {
            // save decimals
            if (dec)
            {
                if (d < e)
                {
                    if (ch != '0')
                    {
                        // fill '0'
                        while (zeros--) *d++ = 0;
                        zeros = 0;

                        // save decimal
                        *d++ = ch - '0';
                    }
                    else zeros++;
                }
            }
            else lhs = (lhs << 1) + (ch - '0');
        }
        else break;

        s++;
    }



    // compute decimal
    while (d-- > decimals) rhs = (rhs + *d) / 2;

    // merge
    return (sign? ((double)lhs + rhs) * -1. : ((double)lhs + rhs));
}

double t_s8_to_d(char const* s)
{
    // skip space
    while (t_isspace(*s)) s++;

    // has sign?
    int sign = 0;
    if (*s == '-')
    {
        sign = 1;
        s++;
    }
    // skip '+'
    else if (*s == '+') s++;

    // nan?
    if (s[0] == 'n' && s[1] == 'a' && s[2] == 'n')
        return TB_NAN;

    // inf or -inf?
    if (s[0] == 'i' && s[1] == 'n' && s[2] == 'f')
        return sign? -TB_INF : TB_INF;

    // skip '0'
    while ((*s) == '0') s++;

    // compute double: lhs.rhs
    int    dec = 0;
    unsigned long lhs = 0;
    double rhs = 0.;
    int    zeros = 0;
    char   decimals[256];
    char*  d = decimals;
    char*  e = decimals + 256;
    while (*s)
    {
        char ch = *s;

        // is the part of decimal?
        if (ch == '.')
        {
            if (!dec)
            {
                dec = 1;
                s++;
                continue ;
            }
            else break;
        }

        // parse integer & decimal
        if (t_isdigit8(ch))
        {
            // save decimals
            if (dec)
            {
                if (d < e)
                {
                    if (ch != '0')
                    {
                        // fill '0'
                        while (zeros--) *d++ = 0;
                        zeros = 0;

                        // save decimal
                        *d++ = ch - '0';
                    }
                    else zeros++;
                }
            }
            else lhs = (lhs << 3) + (ch - '0');
        }
        else break;

        s++;
    }



    // compute decimal
    while (d-- > decimals) rhs = (rhs + *d) / 8;

    // merge
    return (sign? ((double)lhs + rhs) * -1. : ((double)lhs + rhs));
}

double t_s10_to_d(char const* s)
{


    // skip space
    while (t_isspace(*s)) s++;

    // has sign?
    int sign = 0;
    if (*s == '-')
    {
        sign = 1;
        s++;
    }
    // skip '+'
    else if (*s == '+') s++;

    // nan?
    if (s[0] == 'n' && s[1] == 'a' && s[2] == 'n')
        return TB_NAN;

    // inf or -inf?
    if (s[0] == 'i' && s[1] == 'n' && s[2] == 'f')
        return sign? -TB_INF : TB_INF;

    // skip '0'
    while ((*s) == '0') s++;

    // compute double: lhs.rhs
    int    dec = 0;
    unsigned long lhs = 0;
    double rhs = 0.;
    int    zeros = 0;
    char   decimals[256];
    char*  d = decimals;
    char*  e = decimals + 256;
    while (*s)
    {
        char ch = *s;

        // is the part of decimal?
        if (ch == '.')
        {
            if (!dec)
            {
                dec = 1;
                s++;
                continue ;
            }
            else break;
        }

        // parse integer & decimal
        if (t_isdigit10(ch))
        {
            // save decimals
            if (dec)
            {
                if (d < e)
                {
                    if (ch != '0')
                    {
                        // fill '0'
                        while (zeros--) *d++ = 0;
                        zeros = 0;

                        // save decimal
                        *d++ = ch - '0';
                    }
                    else zeros++;
                }
            }
            else lhs = lhs * 10 + (ch - '0');
        }
        else break;

        s++;
    }



    // compute decimal
    while (d-- > decimals) rhs = (rhs + *d) / 10;

    // merge
    return (sign? ((double)lhs + rhs) * -1. : ((double)lhs + rhs));
}

double t_s16_to_d(char const* s)
{


    // skip space
    while (t_isspace(*s)) s++;

    // has sign?
    int sign = 0;
    if (*s == '-')
    {
        sign = 1;
        s++;
    }
    // skip '+'
    else if (*s == '+') s++;

    // nan?
    if (s[0] == 'n' && s[1] == 'a' && s[2] == 'n')
        return TB_NAN;

    // inf or -inf?
    if (s[0] == 'i' && s[1] == 'n' && s[2] == 'f')
        return sign? -TB_INF : TB_INF;

    // skip "0x"
    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
        s += 2;

    // compute double: lhs.rhs
    int    dec = 0;
    unsigned long lhs = 0;
    double rhs = 0.;
    int    zeros = 0;
    char   decimals[256];
    char*  d = decimals;
    char*  e = decimals + 256;
    while (*s)
    {
        char ch = *s;

        // is the part of decimal?
        if (ch == '.')
        {
            if (!dec)
            {
                dec = 1;
                s++;
                continue ;
            }
            else break;
        }

        // parse integer & decimal
        if (t_isdigit10(ch))
        {
            // save decimals
            if (dec)
            {
                if (d < e)
                {
                    if (ch != '0')
                    {
                        // fill '0'
                        while (zeros--) *d++ = 0;
                        zeros = 0;

                        // save decimal
                        *d++ = ch - '0';
                    }
                    else zeros++;
                }
            }
            else lhs = (lhs << 4) + (ch - '0');
        }
        else if (ch > ('a' - 1) && ch < ('f' + 1))
        {
            // save decimals
            if (dec)
            {
                if (d < e)
                {
                    if (ch != '0')
                    {
                        // fill '0'
                        while (zeros--) *d++ = 0;
                        zeros = 0;

                        // save decimal
                        *d++ = (ch - 'a') + 10;
                    }
                    else zeros++;
                }
            }
            else lhs = (lhs << 4) + (ch - 'a') + 10;
        }
        else if (ch > ('A' - 1) && ch < ('F' + 1))
        {
            // save decimals
            if (dec)
            {
                if (d < e)
                {
                    if (ch != '0')
                    {
                        // fill '0'
                        while (zeros--) *d++ = 0;
                        zeros = 0;

                        // save decimal
                        *d++ = (ch - 'A') + 10;
                    }
                    else zeros++;
                }
            }
            else lhs = (lhs << 4) + (ch - 'A') + 10;
        }
        else break;

        s++;
    }



    // compute decimal
    while (d-- > decimals) rhs = (rhs + *d) / 16;

    // merge
    return (sign? ((double)lhs + rhs) * -1. : ((double)lhs + rhs));
}

double t_s_to_d(char const* s)
{


    // skip space
    char const* p = s;
    while (t_isspace(*p)) p++;

    // has sign?
    if (*p == '-' || *p == '+') p++;

    // is hex?
    if (*p++ == '0')
    {
        if (*p == 'x' || *p == 'X')
            return t_s16_to_d(s);
        else if (*p == 'b' || *p == 'B')
            return t_s2_to_d(s);
        else return t_s8_to_d(s);
    }
    else return t_s10_to_d(s);
}

double t_sb_to_d(char const* s, int base)
{


    // the convect functions
    static double (*s_conv[])(char const*) =
    {
        0
    ,   0
    ,   t_s2_to_d
    ,   0
    ,   0
    ,   0
    ,   0
    ,   0
    ,   t_s8_to_d
    ,   0
    ,   t_s10_to_d
    ,   0
    ,   0
    ,   0
    ,   0
    ,   0
    ,   t_s16_to_d
    };
    if(base > (sizeof(s_conv)/sizeof(s_conv[0])) ||
            !s_conv[base])
        return 0;

    // convect it
    return s_conv[base](s);
}


/*********************************string ***********************/
unsigned long t_strlen(const char *s)
{
    char const *p = s;
    while(1)
    {
        if(!p[0])   return (p - s + 0);
        if(!p[1])   return (p - s + 1);
        if(!p[2])   return (p - s + 2);
        if(!p[3])   return (p - s + 3);

        p += 4;
    }
}
