#ifndef __STROPT_H
#define __STROPT_H

#ifdef __cplusplus
extern "C"{
#endif


/*
* @brief 赋值一个字符串
*/
char *str_strdup(const char *str);

/*
*@brief 删除所有空格
*/
void DelSpaceFromString(char *str);

/*
*@brief 删除字符串中的空格。但是只保留一个
*/
char *deblank(char *str);

/*
* @brief 去掉开头结尾的\r\n等
*/
char *str_Trim(char *val,const char *cfg);

/*
* @brief 从str字符串中的第几个位置搜索from
* @params
*       str[in] 字符串
*       seq[in] 搜索的字符
*       from[in] 从第几个开始搜索
*/
int find_bychar(const char *str,char seq,unsigned int from);

/*从后往前搜索*/
int rfind_bychar(const char *str,char seq,unsigned int from);

/*
* @brief 从str字符串中的第from位置搜索seq
* @params
*       str[in] 字符串
*       seq[in] 搜索的字符串
*       from[in] 从第几个开始搜索
*/
int find_bystr(const char *str,char *seq,unsigned int from);

/*从后往前搜索*/
int rfind_bystr(const char *str,char *seq,unsigned int from);

/*
* @brief 区间串 postion + n
* @params
*       n[in] 负数-到串结尾 >串长，取到结尾。
*/
void str_substr(const char *str,char *output,unsigned int position,int n);

/*
* @brief 分割层级传 xx/xx/xx/ 以/分割
* @prams
*/
char *str_strPathSep(char *path,char *output,int level,char sep);

/*
* @brief 获取逻辑行
获得一个逻辑行, 如果某行的尾部由连接符 "\\" 连接，则将下一行合并至本行,
 * 同时将一行字符串中的转义字符回车换行("\r\n" or "\n")去掉
*/
char *str_getline(char **src);

/**该函数会破坏src原来的数据
 ==  Description:将给定字符串用另一个分隔符字符串进行分割
 ==  Argument:[IN] src: 需要被分割的字符串的地址的指针，必须是非空指针
            [IN] delim: 分隔符，非空字符串
               [IN/OUT]argv:字符类型*数组指针至字符[],指针直接指向src地址内部
               [IN]size:整数类型统计参数个数
 ==  Return :	 0: 分割没成功  >0: 分割成功的个数
*/
 int str_strtoks( char *src,const char *delim,char *argv[], int size);

/**该函数不会破坏src原来的数据
 ==  Description:将给定字符串用另一个分隔符字符串进行分割
 ==  Argument:[IN] src: 需要被分割的字符串的地址的指针，必须是非空指针
            [IN] delim: 分隔符，非空字符串
               [IN/OUT]argv:字符类型二维数组,内存由用户分配
               [IN]size:整数类型统计参数个数
 ==  Return :	 0: 分割没成功  >0: 分割成功的个数
*/
int str_strtoks2(const char *src,const char *delim,char **argv, int size);

/* @param str         要被分割的字符串
 * @param delims      分隔符，支持多个 ",\n,"
 * @param save_ptr    Used internally by the function to record the start of
 *                    the string after the last returned token so that search
 *                    can continue.
 * @param delim_found Returns the delimeter that preceeded the token returned
 *
 * @return A pointer to the next token found. This may be the empty string if
 *         consequtive delimeters are found. It will be NULL after the last
 *         token has been found.
 */
char* estrtok_r(char *const str, char const *const delims, char **save_ptr, char *const delim_found);


int my_atoi(const char *s);

long int my_atol(const char *s);

#ifdef __cplusplus
}
#endif

#endif
