#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "stropt.h"

#define MIN(a,b)    ((a) < (b) ? (a) : (b))

//action! 注意 free释放该返回值
char *str_strdup(const char *str)
{
    unsigned int len;
    char *copy = NULL;

    len = strlen(str) + 1;
    if(! (copy = (char *)malloc(len)))
    {
        return NULL;
    }

    memcpy(copy,str,len);

    return copy;
}

//删除字符串所有空格
void DelSpaceFromString(char *str)
{
    int i = 0;
    while('\0' != str[i])
    {
        if(' ' == str[i])
        {
            for(int j = i ; j < strlen(str) ; j ++)
            {
                str[j] = str[j + 1];
            }
            continue;
        }
        i ++;
    }

    str[i] = '\0';
}

char *deblank(char * str)
{
    char * left = str;//前面的指针，负责赋值
    char * right = str;//后面寻找非空格字符的指针

    while (*right)//截至字符串完
    {
        if (*right != ' ')
        {
            if (left<right)
            {
                *left = *right;//赋值
            }
            left++;
        }
        if (*right == ' '&&*(right + 1) != ' ')//寻找到下一个单词时，对left的操作
        {
            *left = ' ';
            left++;
        }
        right++;
    }
    *left = '\0';
    return *str != ' ' ?str:(str + 1);//防止首单词之前有空格现象
}

/*cfg = "\n\r\t" */
char *str_Trim(char *val,const char *cfg)
{
    if(NULL == val)
    {
        return NULL;
    }
    if(NULL == cfg)
    {
        return val;
    }

    int beg = -1,end = -1;
    int val_size = strlen(val);
    int cfg_size = strlen(cfg);

    unsigned int iS = 0;
    unsigned int iC = 0;
    for(iS = 0,iC = 0; iS < val_size; iS++)
    {
        for(iC = 0; iC < cfg_size && val[iS] != cfg[iC]; iC++)
        {
            /* nothing */
            ;
        }

        if(iC < cfg_size)
        {
            continue;
        }

        if(beg < 0)
        {
            beg = iS;
        }

        end = iS;
    }

    if(beg >= 0)
    {
        //printf("end = %d beg = %d\n",end,beg);
        val[end + 1] = '\0';
        return (val + beg);
    }
    return val;
}



/*
* @brief 从str字符串中的第几个位置搜索from
* @params
*       str[in] 字符串
*       seq[in] 搜索的字符
*       from[in] 从第几个开始搜索
*/
int find_bychar(const char *str,char seq,unsigned int from)
{
    if(NULL == str)
    {
        return -1;
    }

    int str_len = strlen(str);
    if(from >= str_len)
    {
        return -1;
    }

    char *location = (char *)str + from;
    char *ptr = strchr(location,seq);
    if(NULL == ptr)
    {
        return -1;
    }

    return (ptr - str); /*返回在字符串中的位置*/
}

/*从后往前搜索*/
int rfind_bychar(const char *str,char seq,unsigned int from)
{
    if(NULL == str)
    {
        return -1;
    }

    int str_len = strlen(str);
    if(from >= str_len)
    {
        return -1;
    }

    char *location = (char *)str + from;
    char *ptr = strrchr(location,seq);
    if(NULL == ptr)
    {
        return -1;
    }

    return (ptr - location);
}

void str_substr(const char *str,char *output,unsigned int position,int n)
{
    if(NULL == str || NULL == output)
    {
        return ;
    }
    int str_len = strlen(str);
    if(position >= str_len)
    {
        return ;
    }

    if(n > 0 && (position + n) > str_len)
    {
        //return ;
        n = -1; //到末尾
    }

    char *begin = (char *)str;
    if(n < 0)
    {
        memcpy(output,begin + position,str_len - position);
    }
    else
    {
        memcpy(output,begin + position,n);
    }
}

char *str_strPathSep(char *path,char *output,int level,char sep)
{
    if(NULL == path)
    {
        return NULL;
    }

    int an_dir = 0;
    int t_lev = 0;
    int t_dir;

    int str_len = strlen(path);

    while(1)
    {
       // printf("an_dir = %d \n",an_dir);
        t_dir = find_bychar(path,sep,an_dir);
        if(t_dir == -1)
        {
            if(t_lev != level)
            {
                return NULL;
            }
            /* t_lev == level */
            if(output)
            {
                str_substr(path,output,an_dir,(str_len - an_dir));
                //printf("1- %s \n",output);
                return output;
            }
            else
                return NULL;
        }
        else if(t_lev == level)
        {
            if(output)
            {
                //printf(" %d %d %d \n",t_lev,t_dir,an_dir);
                str_substr(path,output,an_dir,t_dir - an_dir);
                //printf("2-%s \n",output);
                return output;
            }
            else
                return NULL;
        }

        an_dir = t_dir + 1;
        t_lev++;
    }

    return NULL;
}

char *str_getline(char **src)
{
    char *start = *src;
    char *end = *src;

    int squash = 0;
    int nr = 0;

    if(NULL == start)
    {
        return NULL;
    }

    while(*end)
    {
        switch(*end)
        {
        case '\\':
            squash = 1;
            break;
        case '\r':
            nr++;
            break;
        case '\n':
            if(squash == 0)
            {
                break;
            }
            memmove(end - (squash + nr),end + 1,strlen(end + 1));
        default:
            squash = 0;
            nr = 0;
            break;
        }

        end++;
    }

    if(*end == '\n')
    {
        *(end - nr) = 0;
        *src = end + 1;
    }
    else
    {
        *src = 0;
    }

    return (start);
}


/**
 * 将给定字符串用另一个分隔符字符串进行分割
 * @param src {char**} 需要被分割的字符串的地址的指针，必须是非空指针，
 *  可以是空字符串，此时该函数返回 NULL
 * @param sep {const char*} 分隔符，非空字符串
 * @return {char*} 当前被分割的字符串的指针位置，src 指向下一个将要进行
 *  分隔的起始位置；
 *  1）当返回 NULL 时，则表示分隔过程结束，此时 src 的指针位置被赋予 '\0'；
 *  2）当返回非 NULL 时，则此时 src 指向的字符串可能是或不是空字符串，
 *     如果指向空字符串，则再次分隔时函数肯定能返回 NULL，否则，当再次分隔时
 *     函数返回非 NULL 指针
 *  举例: 源字符串："abcd=|efg=|hijk", 分隔符 "=|"，则第一次分隔后
 *  src 将指向 "efg"，而返回的地址为 "abcd"
 */
char *str_strtok(char **src, const char *sep)
{
    char   *start = *src;
    char   *end;

    /*
     * Skip over leading delimiters.
     */
    start += strspn(start, sep);
    if (*start == 0) {
        *src = start;
        return (0);
    }

    /*
     * Separate off one token.
     */
    end = start + strcspn(start, sep);
    if (*end != 0)
        *end++ = 0;
    *src = end;
    return (start);
}


int str_strtoks(char *src,const char *delim,char *argv[], int size)
{
    char *arg;
    char *start = src;
    int i = 0;
    while ((arg = str_strtok(&start, delim)) != 0)
    {

        argv[i++] = arg;

        if(i >= size )
            break;
    }

    return i;

}

/*不破坏原来的src 数据,argv缓存由调用者分配*/
int str_strtoks2(const char *src,const char *delim,char **argv, int size)
{
    char *arg;
    char *start = strdup(src);
    int i = 0;
    while ((arg = str_strtok(&start, delim)) != 0)
    {

        strncpy(argv[i],arg, MIN(sizeof(argv[i])-1,strlen(arg)));
        i++;
        if(i >= size )
            break;
    }

    free(start);
    return i;

}
