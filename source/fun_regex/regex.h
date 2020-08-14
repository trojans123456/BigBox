#ifndef __REGEX_H
#define __REGEX_H

/* Supports:
* ---------
*   '.'        Dot, matches any character
*   '^'        Start anchor, matches beginning of string
*   '$'        End anchor, matches end of string
*   '*'        Asterisk, match zero or more (greedy)
*   '+'        Plus, match one or more (greedy)
*   '?'        Question, match zero or one (non-greedy)
*   '[abc]'    Character class, match if one of {'a', 'b', 'c'}
*   '[^abc]'   Inverted class, match if NOT one of {'a', 'b', 'c'} -- NOTE: feature is currently broken!
*   '[a-zA-Z]' Character ranges, the character set of the ranges { a-z | A-Z }
*   '\s'       Whitespace, \t \f \r \n \v and spaces
*   '\S'       Non-whitespace
*   '\w'       Alphanumeric, [a-zA-Z0-9_]
*   '\W'       Non-alphanumeric
*   '\d'       Digits, [0-9]
*   '\D'       Non-digits
*
*
*/

/*
* supports
*   '.'         匹配任意字符
*   '^'         开始锚点 从字符串的开头匹配
*   '$'         结束锚点 从字符串的结尾匹配
*   '*'         匹配0个或多个
*   '+'         匹配1个或多个
*   '?'         匹配0个或1个
*   '[abc]'     匹配[]中的其中一个 'a' 'b' 'c'
*   '[^abc]'    匹配[]中的不包含 'a' 'b' 'c'的 子串
*   '[a-zA-Z]'  范围匹配
*   '\s'        空白格, \t \f \r \n \v and spaces
*   '\S'        非空白格
*   '\w'        字母, [a-zA-Z0-9_]
*   '\W'        非字母
*   '\d'        数字, [0-9]
*   '\D'        非数字
*/

#ifdef __cplusplus
extern "C" {
#endif

/* Typedef'd pointer to get abstract datatype. */
typedef struct regex_t* re_t;


/* Compile regex string pattern to a regex_t-array. */
/* 编译 */
re_t re_compile(const char* pattern);


/* Find matches of the compiled pattern inside text. */
/* 在文本中查找已编译模式的匹配项*/
int  re_matchp(re_t pattern, const char* text, int* matchlenght);


/* Find matches of the txt pattern inside text (will compile automatically first). */
int  re_match(const char* pattern, const char* text, int* matchlenght);


/* eg
 * char *text = "5ac";
 * char *pattern = "\\d";
 * int lenght;
 * int m = re_match(pattern,text,&lenght);
 * if(m == -1)
 *      return false;
 *
 *
 *
 *
*/

#ifdef __cplusplus
}
#endif

#endif
