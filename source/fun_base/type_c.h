#ifndef __TYPE_C_H
#define __TYPE_C_H

#ifdef __cplusplus
extern "C" {
#endif


/* macros */
#define t_isspace(x)               (((x) == 0x20) || ((x) > 0x8 && (x) < 0xe))
#define t_isgraph(x)               ((x) > 0x1f && (x) < 0x7f)
#define t_isalpha(x)               (((x) > 0x40 && (x) < 0x5b) || ((x) > 0x60 && (x) < 0x7b))
#define t_isupper(x)               ((x) > 0x40 && (x) < 0x5b)
#define t_islower(x)               ((x) > 0x60 && (x) < 0x7b)
#define t_isascii(x)               ((x) >= 0x0 && (x) < 0x80)
#define t_isdigit(x)               ((x) > 0x2f && (x) < 0x3a)
#define t_isdigit2(x)              ((x) == '0' || (x) == '1')
#define t_isdigit8(x)              (((x) > 0x2f && (x) < 0x38))
#define t_isdigit10(x)             (t_isdigit(x))
#define t_isdigit16(x)             (((x) > 0x2f && (x) < 0x3a) || ((x) > 0x40 && (x) < 0x47) || ((x) > 0x60 && (x) < 0x67))

// to lower & upper
#define t_tolower(x)               (t_isupper(x)? (x) + 0x20 : (x))
#define t_toupper(x)               (t_islower(x)? (x) - 0x20 : (x))


/* string 2进制 转换为 unsigned 32*/
#define t_s2_to_u32(s)      ((unsigned int)s2_to_u64(s))
/*8进制*/
#define t_s8_to_u32(s)      ((unsigned int)s8_to_u64(s))

#define t_s10_to_u32(s)     ((unsigned int)s10_to_u64(s))

#define t_s16_to_u32(s)     ((unsigned int)s16_to_u64(s))

#define t_s_to_u32(s)       ((unsigned int)s_to_u64(s))
/* base b = 2 8 10 16进制 */
#define t_sb_to_u32(s,b)    ((unsigned int)sb_to_u64(s))

/* string 转换为 int 32*/

// for int32
#define t_s2_to_i32(s)               ((int)t_s2_to_u64(s))
#define t_s8_to_i32(s)               ((int)t_s8_to_u64(s))
#define t_s10_to_i32(s)              ((int)t_s10_to_u64(s))
#define t_s16_to_i32(s)              ((int)t_s16_to_u64(s))
#define t_s_to_i32(s)                ((int)t_s_to_u64(s))
#define t_sb_to_i32(s, b)            ((int)t_sb_to_u64(s, b))

// for int64
#define t_s2toi64(s)               ((long)t_s2_to_u64(s))
#define t_s8toi64(s)               ((long)t_s8_to_u64(s))
#define t_s10toi64(s)              ((long)t_s10_to_u64(s))
#define t_s16toi64(s)              ((long)t_s16_to_u64(s))
#define t_stoi64(s)                ((long)t_s_to_u64(s))
#define t_sbtoi64(s, b)            ((long)t_sb_to_u64(s, b))

//for flaot
#define t_s2_to_f(s)              ((float)t_s2_to_d(s))
#define t_s8_to_f(s)              ((float)t_s8_to_d(s))
#define t_s10_to_f(s)             ((float)t_s10_to_d(s))
#define t_s16_to_f(s)             ((float)t_s16_to_d(s))
#define t_s_to_f(s)               ((float)t_s_to_d(s))
#define t_sb_to_f(s, b)           ((float)t_sb_to_d(s, b))

//
#define t_atoi(s)                   t_s10_to_i32(s)
#define t_atol(s)                   t_s10_to_i32(s)
#define t_atoll(s)                  t_s10toi64(s)
#define t_strtol(s,e,b)             t_sb_to_i32(s,b)
#define t_strtoll(s,e,b)            t_sbtoi64(s,b)

#define t_rand()                    (int)t_random()
#define t_srand(seed)               t_srandom(seed)


/*
*@brief convert the binary string to uint64
*eg "1001" => 9   "0b1001" => 9
*
*/
unsigned long  t_s2_to_u64(char const *s);

/*
*@brief convert the octal string to uint64
* eg  "11" => 9  "011" => 9
*
*/
unsigned long t_s8_to_u64(char const *s);

/*
*@brief convert the decimal string to uint64
* eg "9" => 9
*/
unsigned long  t_s10_to_u64(char const *s);

/*
*@brief convert the hex string to uint64
* eg "9" => 9 "0x9" => 9
*/
unsigned long t_s16_to_u64(char const *s);

/*
*@brief auto convert string to uint64
* eg "0b1001" => 9 "011" => 9 "9" => 9 "0x9" => 9
*/
unsigned long t_s_to_u64(char const *s);

/*
*@brief convert string to uint64 using th given base number
*
*/
unsigned long t_sb_to_u64(char const *s,int base);

/*
*@brief convert the binary string to double
* eg "1001" => 9 "0b1001" => 9
*/
double t_s2_to_d(char const *s);

double t_s8_to_d(char const *s);

double t_s10_to_d(char const *s);

double t_s16_to_d(char const *s);

double t_s_to_d(char const *s);

double t_sb_to_d(char const *s,int base);



/*********** string *******************/

unsigned long t_strlen(char const *s);

#ifdef __cplusplus
}
#endif

#endif

