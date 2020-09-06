#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "http.h"

void parser_free(struct http_url *url)
{
    if(!url)
    {
        if(!url->scheme)    free(url->scheme);
        if(!url->host)      free(url->host);
        if(!url->port)      free(url->port);
        if(!url->path)      free(url->path);
        if(!url->query)     free(url->query);
        if(!url->fragment)  free(url->fragment);
        if(!url->username)  free(url->username);
        if(!url->password)  free(url->password);

        free(url);
    }
}

/*检查字符串是否合法*/
static int is_scheme_char(int c)
{
    return (!isalpha(c) && '+' != c && '-' != c && '.' != c) ? 0 : 1;
}

/**/
static char *hostname_to_ip(char *hostname)
{
    struct hostent *h;
    if((h = gethostbyname(hostname)) == NULL)
    {
        return NULL;
    }
    return inet_ntoa(*(struct in_addr *)h->h_addr);
}

struct http_url *parser_url(const char *url)
{
    struct http_url *purl;

    const char *tmpstr;
    const char *curstr;
    int len;
    int i;
    int userpass_flag;
    int bracket_flag;

    /* Allocate the parsed url storage */
    purl = (struct parsed_url*)calloc(1,sizeof(struct http_url));
    if ( NULL == purl )
    {
        return NULL;
    }

    curstr = url;

    /*
     * <scheme>:<scheme-specific-part>
     * <scheme> := [a-z\+\-\.]+
     *             upper case = lower case for resiliency
     */
    /* Read scheme */
    tmpstr = strchr(curstr, ':');
    if ( NULL == tmpstr )
    {
        parser_free(purl);
        return NULL;
    }

    /* Get the scheme length */
    len = tmpstr - curstr;

    /* Check restrictions */
    for ( i = 0; i < len; i++ )
    {
        if (is_scheme_char(curstr[i]) == 0)
        {
            /* Invalid format */
            parser_free(purl);
            return NULL;
        }
    }
    /* Copy the scheme to the storage */
    purl->scheme = (char*)malloc(sizeof(char) * (len + 1));
    if ( NULL == purl->scheme )
    {
        parser_free(purl);

        return NULL;
    }

    (void)strncpy(purl->scheme, curstr, len);
    purl->scheme[len] = '\0';

    /* Make the character to lower if it is upper case. */
    for ( i = 0; i < len; i++ )
    {
        purl->scheme[i] = tolower(purl->scheme[i]);
    }

    /* Skip ':' */
    tmpstr++;
    curstr = tmpstr;

    /*
     * //<user>:<password>@<host>:<port>/<url-path>
     * Any ":", "@" and "/" must be encoded.
     */
    /* Eat "//" */
    for ( i = 0; i < 2; i++ )
    {
        if ( '/' != *curstr )
        {
            parser_free(purl);
            return NULL;
        }
        curstr++;
    }

    /* Check if the user (and password) are specified. */
    userpass_flag = 0;
    tmpstr = curstr;
    while ( '\0' != *tmpstr )
    {
        if ( '@' == *tmpstr )
        {
            /* Username and password are specified */
            userpass_flag = 1;
            break;
        }
        else if ( '/' == *tmpstr )
        {
            /* End of <host>:<port> specification */
            userpass_flag = 0;
            break;
        }
        tmpstr++;
    }

    /* User and password specification */
    tmpstr = curstr;
    if ( userpass_flag )
    {
        /* Read username */
        while ( '\0' != *tmpstr && ':' != *tmpstr && '@' != *tmpstr )
        {
            tmpstr++;
        }
        len = tmpstr - curstr;
        purl->username = (char*)malloc(sizeof(char) * (len + 1));
        if ( NULL == purl->username )
        {
            parser_free(purl);
            return NULL;
        }
        (void)strncpy(purl->username, curstr, len);
        purl->username[len] = '\0';

        /* Proceed current pointer */
        curstr = tmpstr;
        if ( ':' == *curstr )
        {
            /* Skip ':' */
            curstr++;

            /* Read password */
            tmpstr = curstr;
            while ( '\0' != *tmpstr && '@' != *tmpstr )
            {
                tmpstr++;
            }
            len = tmpstr - curstr;
            purl->password = (char*)malloc(sizeof(char) * (len + 1));
            if ( NULL == purl->password )
            {
                parser_free(purl);
                return NULL;
            }
            (void)strncpy(purl->password, curstr, len);
            purl->password[len] = '\0';
            curstr = tmpstr;
        }
        /* Skip '@' */
        if ( '@' != *curstr )
        {
            parser_free(purl);
            return NULL;
        }
        curstr++;
    }

    if ( '[' == *curstr )
    {
        bracket_flag = 1;
    }
    else
    {
        bracket_flag = 0;
    }
    /* Proceed on by delimiters with reading host */
    tmpstr = curstr;
    while ( '\0' != *tmpstr ) {
        if ( bracket_flag && ']' == *tmpstr )
        {
            /* End of IPv6 address. */
            tmpstr++;
            break;
        }
        else if ( !bracket_flag && (':' == *tmpstr || '/' == *tmpstr) )
        {
            /* Port number is specified. */
            break;
        }
        tmpstr++;
    }
    len = tmpstr - curstr;
    purl->host = (char*)malloc(sizeof(char) * (len + 1));
    if ( NULL == purl->host || len <= 0 )
    {
        parser_free(purl);
        return NULL;
    }
    (void)strncpy(purl->host, curstr, len);
    purl->host[len] = '\0';
    curstr = tmpstr;

    /* Is port number specified? */
    if ( ':' == *curstr )
    {
        curstr++;
        /* Read port number */
        tmpstr = curstr;
        while ( '\0' != *tmpstr && '/' != *tmpstr )
        {
            tmpstr++;
        }
        len = tmpstr - curstr;
        purl->port = (char*)malloc(sizeof(char) * (len + 1));
        if ( NULL == purl->port )
        {
            parser_free(purl);
            return NULL;
        }
        (void)strncpy(purl->port, curstr, len);
        purl->port[len] = '\0';
        curstr = tmpstr;
    }
    else
    {
        purl->port = "80";
    }

    /* Get ip */
    char *ip = hostname_to_ip(purl->host);
    purl->ip = ip;

    /* Set uri */
    purl->uri = (char*)url;

    /* End of the string */
    if ( '\0' == *curstr )
    {
        return purl;
    }

    /* Skip '/' */
    if ( '/' != *curstr )
    {
        parser_free(purl);
        return NULL;
    }
    curstr++;

    /* Parse path */
    tmpstr = curstr;
    while ( '\0' != *tmpstr && '#' != *tmpstr  && '?' != *tmpstr )
    {
        tmpstr++;
    }
    len = tmpstr - curstr;
    purl->path = (char*)malloc(sizeof(char) * (len + 1));
    if ( NULL == purl->path )
    {
        parser_free(purl);
        return NULL;
    }
    (void)strncpy(purl->path, curstr, len);
    purl->path[len] = '\0';
    curstr = tmpstr;

    /* Is query specified? */
    if ( '?' == *curstr )
    {
        /* Skip '?' */
        curstr++;
        /* Read query */
        tmpstr = curstr;
        while ( '\0' != *tmpstr && '#' != *tmpstr )
        {
            tmpstr++;
        }
        len = tmpstr - curstr;
        purl->query = (char*)malloc(sizeof(char) * (len + 1));
        if ( NULL == purl->query )
        {
            parser_free(purl);
            return NULL;
        }
        (void)strncpy(purl->query, curstr, len);
        purl->query[len] = '\0';
        curstr = tmpstr;
    }

    /* Is fragment specified? */
    if ( '#' == *curstr )
    {
        /* Skip '#' */
        curstr++;
        /* Read fragment */
        tmpstr = curstr;
        while ( '\0' != *tmpstr )
        {
            tmpstr++;
        }
        len = tmpstr - curstr;
        purl->fragment = (char*)malloc(sizeof(char) * (len + 1));
        if ( NULL == purl->fragment )
        {
            parser_free(purl);
            return NULL;
        }
        (void)strncpy(purl->fragment, curstr, len);
        purl->fragment[len] = '\0';
        curstr = tmpstr;
    }
    return purl;
}


/*** str opt **/
/**
 * @brief 获取一个字符串在另一个字符串中的偏移量
 * @param a
 * @param b
 * @return
 */
int str_index_of(const char *a,char *b)
{
    char *offset = (char *)strstr(a,b);
    return offset - a;
}

/**
 * @brief 检查一个字符串是否包含另一个字符串
 * @param haystack
 * @param needle
 * @return
 */
int str_contains(const char *haystack,const char *needle)
{
    char *pos = (char *)strstr(haystack,needle);
    if(pos)
        return 1;
    else
        return 0;
}

/**
 * @brief 从字符串中删除最后一个字符
 * @param string
 * @param to_trim
 * @return
 */
char *trim_end(char *string,char to_trim)
{
    char last_char = string[strlen(string) - 1];
    if(last_char == to_trim)
    {
        char *new_string = string;
        new_string[strlen(string) - 1] = 0;
        return new_string;
    }
    else
    {
        return string;
    }
}

char *str_cat(char *a,char *b)
{
    char *target = (char *)malloc(strlen(a) + strlen(b) + 1);
    strcpy(target,a);
    strcat(target,b);
    return target;
}

/**
 * @brief 将整数值转换为十六进制字符
 * @param code
 * @return
 */
char to_hex(char code)
{
    static char hex[] = "0123456789abcdef";
    return hex[code & 15];
}


/*
    URL encodes a string
*/
char *urlencode(char *str)
{
    char *pstr = str, *buf = (char*)malloc(strlen(str) * 3 + 1), *pbuf = buf;
    while (*pstr)
    {
        if (isalnum(*pstr) || *pstr == '-' || *pstr == '_' || *pstr == '.' || *pstr == '~')
            *pbuf++ = *pstr;
        else if (*pstr == ' ')
            *pbuf++ = '+';
        else
            *pbuf++ = '%', *pbuf++ = to_hex(*pstr >> 4), *pbuf++ = to_hex (*pstr & 15);
        pstr++;
    }
    *pbuf = '\0';
    return buf;
}

/*
    Replacement for the string.h strndup, fixes a bug
*/
char *str_ndup (const char *str, size_t max)
{
    size_t len = strnlen (str, max);
    char *res = (char*)malloc (len + 1);
    if (res)
    {
        memcpy (res, str, len);
        res[len] = '\0';
    }
    return res;
}

/*
    Replacement for the string.h strdup, fixes a bug
*/
char *str_dup(const char *src)
{
   char *tmp = (char*)malloc(strlen(src) + 1);
   if(tmp)
       strcpy(tmp, src);
   return tmp;
}

/*
    Search and replace a string with another string , in a string
*/
char *str_replace(char *search , char *replace , char *subject)
{
    char  *p = NULL , *old = NULL , *new_subject = NULL ;
    int c = 0 , search_size;
    search_size = strlen(search);
    for(p = strstr(subject , search) ; p != NULL ; p = strstr(p + search_size , search))
    {
        c++;
    }
    c = ( strlen(replace) - search_size )*c + strlen(subject);
    new_subject = (char*)malloc( c );
    strcpy(new_subject , "");
    old = subject;
    for(p = strstr(subject , search) ; p != NULL ; p = strstr(p + search_size , search))
    {
        strncpy(new_subject + strlen(new_subject) , old , p - old);
        strcpy(new_subject + strlen(new_subject) , replace);
        old = p + search_size;
    }
    strcpy(new_subject + strlen(new_subject) , old);
    return new_subject;
}

/*
    Get's all characters until '*until' has been found
*/
char* get_until(char *haystack, char *until)
{
    int offset = str_index_of(haystack, until);
    return str_ndup(haystack, offset);
}


/* decodeblock - decode 4 '6-bit' characters into 3 8-bit binary bytes */
void decodeblock(unsigned char in[], char *clrstr)
{
    unsigned char out[4];
    out[0] = in[0] << 2 | in[1] >> 4;
    out[1] = in[1] << 4 | in[2] >> 2;
    out[2] = in[2] << 6 | in[3] >> 0;
    out[3] = '\0';
    strncat((char *)clrstr, (char *)out, sizeof(out));
}

/*
    Decodes a Base64 string
*/
char* base64_decode(char *b64src)
{
    char *clrdst = (char*)malloc( ((strlen(b64src) - 1) / 3 ) * 4 + 4 + 50);
    char b64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int c, phase, i;
    unsigned char in[4];
    char *p;
    clrdst[0] = '\0';
    phase = 0; i=0;
    while(b64src[i])
    {
        c = (int) b64src[i];
        if(c == '=')
        {
            decodeblock(in, clrdst);
            break;
        }
        p = strchr(b64, c);
        if(p)
        {
            in[phase] = p - b64;
            phase = (phase + 1) % 4;
            if(phase == 0)
            {
                decodeblock(in, clrdst);
                in[0]=in[1]=in[2]=in[3]=0;
            }
        }
        i++;
    }
    clrdst = (char*)realloc(clrdst, strlen(clrdst) + 1);
    return clrdst;
}

/* encodeblock - encode 3 8-bit binary bytes as 4 '6-bit' characters */
void encodeblock( unsigned char in[], char b64str[], int len )
{
    char b64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    unsigned char out[5];
    out[0] = b64[ in[0] >> 2 ];
    out[1] = b64[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ];
    out[2] = (unsigned char) (len > 1 ? b64[ ((in[1] & 0x0f) << 2) |
             ((in[2] & 0xc0) >> 6) ] : '=');
    out[3] = (unsigned char) (len > 2 ? b64[ in[2] & 0x3f ] : '=');
    out[4] = '\0';
    strncat((char *)b64str, (char *)out, sizeof(out));
}

/*
    Encodes a string with Base64
*/
char* base64_encode(char *clrstr)
{
    char *b64dst = (char*)malloc(strlen(clrstr) + 50);
    char b64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    unsigned char in[3];
    int i, len = 0;
    int j = 0;

    b64dst[0] = '\0';
    while(clrstr[j])
    {
        len = 0;
        for(i=0; i<3; i++)
        {
            in[i] = (unsigned char) clrstr[j];
            if(clrstr[j])
            {
                len++; j++;
            }
            else in[i] = 0;
        }
        if( len )
        {
            encodeblock( in, b64dst, len );
        }
    }
    b64dst = (char*)realloc(b64dst, strlen(b64dst) + 1);
    return b64dst;
}

/******************* http request *************************/
const char *http_req_type_char[] =
{
    "GET",
    "OPTIONS",
    "HEAD",
    "POST",
    "PUT",
    "DELETE",
    "TRACE",
    "CONNECT",
    "PROPFIND",
    "PROPPATCH",
    "MKCOL",
    "COPY",
    "MOVE",
    "LOCK",
    "UNLOCK",
    NULL
};

char *http_header_get_value(http_header_list *list, const char *name)
{
    int i = 0;
    char *l_return = NULL;
    if(name == NULL)
        return NULL;

    for(i = 0;i < HTTP_HEADER_MAX;i++)
    {
        if(list->header[i] &&
           (strcasecmp(list->header[i],name) == 0))
        {
            if(list->value[i] == NULL)
                return NULL;
            l_return = list->value[i];
            break;
        }
    }
    return l_return;
}

char *http_strdup(const char *str)
{
    int len = strlen(str);
    char *val = (char *)malloc(len + 3); /* 2 \r\n + 1 \0 */
    snprintf(val,len + 2,"%s\r\n",str);
    return val;
}

int http_header_set_value(http_header_list *list, const char *name, const char *value)
{
    int i = 0;
    char *l_temp_value = NULL;
    int l_return = 0;

    if(list == NULL || name == NULL || value == NULL)
        return -1;

    l_temp_value = http_header_get_value(list,name);
    if(l_temp_value == NULL)
    {
        for(i = 0;i < HTTP_HEADER_MAX;i++)
        {
            if(list->header[i] == NULL)
            {
                list->header[i] = http_strdup(name);
                list->value[i] = http_strdup(value);
                l_return = 1;
                break;
            }
        }
    }
    else
    {
        for(i = 0;i < HTTP_HEADER_MAX;i++)
        {
            if(strcasecmp(list->value,l_temp_value) == 0)
            {
                free(list->value[i]);
                list->value[i] = http_strdup(value);
                l_return = 1;
                break;
            }
        }
    }
    return l_return;
}

int http_header_clear_value(http_header_list *list, const char *name)
{
    int i = 0;
    int l_return = 0;
    if(list == NULL || name == NULL)
        return -1;

    for(i = 0;i < HTTP_HEADER_MAX;i++)
    {
        if(list->header[i] &&
           (strcasecmp(list->header[i],name) == 0))
        {
            free(list->header[i]);
            list->header = NULL;

            free(list->value[i]);
            list->value = NULL;

            l_return = 1;
            break;
        }
    }
    return l_return;
}

/*********** request ****************/
struct http_request *http_request_new()
{
    struct http_request *req = (struct http_request *)calloc(1,sizeof(struct http_request));
    if(!req)
        return NULL;

    req->http_ver = 1.1;
}

void http_request_delete(http_request *req)
{
    if(req)
    {
        if(req->host)    free(req->host);

        if(req->resource) free(req->resource);
        if(req->body)   free(req->body);

        free(req);
    }
}

int http_request_add_method(struct http_request *req,int method)
{
    req->method = method;
}

int http_request_add_header(struct http_request *req,const char *name,const char *value)
{
    return http_header_set_value(&req->header_list,name,value);
}
