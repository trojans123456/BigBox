#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "string_dic_p.h"
#include "ini_config1.h"
#if 0
struct config_ini__
{
    string_dic_t *d;
};
#endif
/**
 * @brief 转换为小写字母
 * @param s
 */
static void strlwc(char *s)
{
    int i;
    if(s == NULL)
        return ;
    i = 0;
    while(s[i])
    {
        s[i] == (char)tolower((int)s[i]);
        i++;
    }
}

/**
 * @brief 复制一个字符串
 * @param s
 * @return
 */
static char * xstrdup(const char * s)
{
    char *t = NULL;
    size_t len = 0;
    if(!s)
        return NULL;

    len = strlen(s) + 1;
    t = (char *)calloc(1,len);
    if(t)
    {
        memcpy(t,s,len);
    }
    return t;
}

/**
 * @brief 删除字符串开头和结尾的空格
 * @param s
 */
static void strstrip(char *s)
{
    char *last, *dest;
    if (s==NULL) return ;

    last = s + strlen(s);
    dest = s;

    while (isspace((int)*s) && *s) s++;
    while (last > s) {
        if (!isspace((int)*(last-1)))
            break ;
        last -- ;
    }
    *last = (char)0;

    memmove(dest, s, last - s + 1);
}


#define ASCIILINESZ         (1024)

/**
 * This enum stores the status for each parsed line (internal use only).
 */
typedef enum _line_status_ {
    LINE_UNPROCESSED,
    LINE_ERROR,
    LINE_EMPTY,
    LINE_COMMENT,
    LINE_SECTION,
    LINE_VALUE
} line_status ;

/*-------------------------------------------------------------------------*/
/**
  @brief    Load a single line from an INI file
  @param    input_line  Input line, may be concatenated multi-line input
  @param    section     Output space to store section
  @param    key         Output space to store key
  @param    value       Output space to store value
  @return   line_status value
 */
/*--------------------------------------------------------------------------*/
static line_status iniparser_line(
    int line_size,
    const char * input_line,
    char ** section_out,
    char ** key_out,
    char ** value_out)
{
    line_status sta ;
    int len = line_size-1;
    char * line = (char *)malloc(line_size);
    char * key = NULL;
    char * value = NULL;
    char * equals = NULL;

    if (!line) {
        fprintf(stderr, "iniparser: memory alloc error\n");
        return LINE_ERROR;
    }

    *line = 0;


    strcpy(line, input_line);
    strstrip(line);
    len = (int)strlen(line);

    /* only allocate necessary space for key & val */
    equals = strchr(line, '=');
    if (equals) {
        value = (char *)malloc((len + line) - equals + 1);
        key = (char *)malloc(equals - line + 1);
       *value = 0;
    } else {
        key = (char *)malloc(line_size + 1);
    }

    if (!key || (equals && !value)) {
        fprintf(stderr, "iniparser: memory alloc error\n");
        sta = LINE_ERROR;
        goto out;
    }

    *key = 0;

    sta = LINE_UNPROCESSED ;
    if (len<1) {
        /* Empty line */
        sta = LINE_EMPTY ;
    } else if (line[0]=='#' || line[0]==';') {
        /* Comment line */
        sta = LINE_COMMENT ;
    } else if (line[0]=='[' && line[len-1]==']') {
        /* Section name */
        sscanf(line, "[%[^]]", key);
        strstrip(key);
        strlwc(key);
        sta = LINE_SECTION ;
        *section_out=key;
        /* don't free key's memory */
        key = NULL;
    } else if (equals && (sscanf (line, "%[^=] = \"%[^\"]\"", key, value) == 2
           ||  sscanf (line, "%[^=] = '%[^\']'",   key, value) == 2
           ||  sscanf (line, "%[^=] = %[^;#]",     key, value) == 2)) {
        /* Usual key=value, with or without comments */
        strstrip(key);
        strlwc(key);
        strstrip(value);
        /*
         * sscanf cannot handle '' or "" as empty values
         * this is done here
         */
        if (!strcmp(value, "\"\"") || (!strcmp(value, "''"))) {
            value[0]=0 ;
        }
        *key_out = key;
        *value_out = value;
        key = NULL;
        value = NULL;
        sta = LINE_VALUE ;
    } else if (equals && (sscanf(line, "%[^=] = %[;#]", key, value)==2
           ||  sscanf(line, "%[^=] %[=]", key, value) == 2)) {
        /*
         * Special cases:
         * key=
         * key=;
         * key=#
         */
        strstrip(key);
        strlwc(key);
        value[0]=0 ;
        *key_out = key;
        *value_out = value;

        /* don't free out params key or val's memory */
        key = NULL;
        value = NULL;
        sta = LINE_VALUE ;
    } else {
        /* Generate syntax error */
        sta = LINE_ERROR ;
    }

out:
    if (line) {
        free(line);
        line = NULL;
    }
    if (key) {
        free(key);
        key = NULL;
    }
    if (value) {
        free(value);
        value= NULL;
    }
    return sta ;
}

/**
 * @brief 加载一个ini文件并解析
 * @param a_name
 * @return
 */
config_ini_t *config_init_load(const char *ininame)
{
    FILE * in = NULL ;

    char line    [ASCIILINESZ+1] ;
    char *section = xstrdup("");
    char *current_section = NULL;
    char *key = NULL;
    char *val = NULL;
    char* full_line = NULL;
    char* prev_line = NULL;

    int  len ;
    int  lineno=0 ;
    int  errs=0;
    int  seckey_size=0;

    config_ini_t *a_ini = calloc(1,sizeof(config_ini_t));
    if(!a_ini)
        return NULL;

    //string_dic_t * dict = NULL ;


    if ((in=fopen(ininame, "r"))==NULL) {
        fprintf(stderr, "iniparser: cannot open %s\n", ininame);
        goto out;
    }

    a_ini->d = string_dic_new(0) ;
    if (!a_ini->d) {
        goto out;
    }

    memset(line,    0, ASCIILINESZ);

    while (fgets(line, ASCIILINESZ, in)!=NULL) {
        int prev_line_len = 0;
        int multi_line = 0;
        int total_size = 0;

        if (key) {
            free(key);
            key = NULL;
        }
        if (val) {
            free(val);
            val = NULL;
        }

        lineno++ ;
        len = (int)strlen(line)-1;
        if (len==0)
            continue;
        /* Safety check against buffer overflows */
        if (line[len]!='\n' && !feof(in)) {
            fprintf(stderr,
                    "iniparser: input line too long in %s (%d)\n",
                    ininame,
                    lineno);
            errs++;
            goto out;
        }
        /* Get rid of \n and spaces at end of line */
        while ((len>=0) &&
               ((line[len]=='\n') || (isspace(line[len])))) {
            line[len]=0 ;
            len-- ;
        }

        if (len < 0) { /* Line was entirely \n and/or spaces */
            len = 0;
        }

        /* Detect multi-line */
        if (line[len]=='\\') {
            multi_line = 1;
        }
        if (multi_line) {
            /* Multi-line value */
            /* length without trailing '\' */
            /* remove multi-line indicator before appending*/
            line[len] = 0;
            len--;
        }

        /*
             * If processing a multi-line then append it the previous portion,
             * at this point 'full_line' has the previously read portion of a
             * multi-line line (or NULL)
             */
        prev_line = full_line;
        prev_line_len=0;
        if (prev_line) {
            prev_line_len = strlen(prev_line);
        }

        /* len is not strlen(line) but strlen(line) -1 */
        total_size = (len+1) + prev_line_len + 1;

        full_line = (char *)malloc(total_size);
        if (!full_line) {
            fprintf(stderr,
                    "iniparser: out of mem\n");
            errs++;
            goto out;
        }

        memset(full_line, 0, total_size);

        if (prev_line) {
            strcpy(full_line, prev_line);
        }

        strcpy(full_line + prev_line_len, line);
        free(prev_line);
        prev_line = NULL;

        if (multi_line) {
            continue ;
        }

        switch (iniparser_line(total_size, full_line, &current_section, &key, &val)) {
        case LINE_EMPTY:
        case LINE_COMMENT:
            break ;

        case LINE_SECTION:
            if (section) {
                free(section);
                section=NULL;
            }
            errs = string_dic_set(a_ini->d, current_section, NULL);
            section = current_section;
            break ;

        case LINE_VALUE:
        {
            char *seckey;
            /* section + ':' + key + eos */
            seckey_size = strlen(section) + strlen(key) +2;
            seckey = (char *)malloc(seckey_size);
            if (!seckey) {
                errs++;
                fprintf(stderr,
                        "iniparser: out of mem\n");
                goto out;
            }
            snprintf(seckey, seckey_size, "%s:%s", section, key);
            errs = string_dic_set(a_ini->d, seckey, val) ;
            free(seckey);
            seckey = NULL;
        }
            break ;

        case LINE_ERROR:
            fprintf(stderr, "iniparser: syntax error in %s (%d):\n",
                    ininame,
                    lineno);
            fprintf(stderr, "-> %s\n", full_line);
            errs++ ;
            break;

        default:
            break ;
        }
        memset(line, 0, ASCIILINESZ);
        if (full_line) {
            free(full_line);
            full_line = NULL;
        }
        if (errs<0) {
            fprintf(stderr, "iniparser: memory allocation failure\n");
            break ;
        }
    }
out:
    if (errs) {
        string_dic_del(a_ini->d);
        a_ini->d = NULL ;
    }
    if (val) {
        free(val);
        val = NULL;
    }
    if (key) {
        free(key);
        key = NULL;
    }
    if (section) {
        free(section);
        section = NULL;
    }
    if (full_line) {
        free(full_line);
        full_line = NULL;
    }
    if (prev_line) {
        free(prev_line);
        prev_line = NULL;
    }
    if (in) {
        fclose(in);
    }
    return a_ini ;
}

/**
 * @brief 释放加载的ini对象
 * @param a_ini
 */
void config_ini_free(config_ini_t *a_ini)
{
    string_dic_del(a_ini->d);
    free(a_ini);
}

/**
 * @brief 初始化一个对象
 * @param a_ini
 */
config_ini_t * config_ini_new()
{
    config_ini_t *a_ini = (config_ini_t *)calloc(1,sizeof(config_ini_t));
    if(!a_ini)
        return NULL;

    a_ini->d = string_dic_new(0);
    if(!a_ini->d)
    {
        free(a_ini);
        return NULL;
    }

    return a_ini;
}

/**
 * @brief 保存到文件
 * @param a_name
 */
int config_ini_save(config_ini_t *a_ini,const char *a_name)
{
    if(!a_ini || !a_ini->d || !a_name)
        return -1;

    FILE *f = fopen(a_name,"w+");
    if(!f)
        return -1;

    config_ini_save_ini(a_ini,f);

    fclose(f);

    return 0;
}

/**
 * @brief 获取文件中的section节数
 * @param a_ini
 * @return
 */
int config_ini_get_section_num(config_ini_t *a_ini)
{
    int i;
    int nsec;
    if(a_ini == NULL || a_ini->d == NULL)
        return -1;

    nsec = 0;
    for(i = 0;i < a_ini->d->size;i++)
    {
        if(a_ini->d->key[i] == NULL)
            continue;
        /* section 以[]标记 */
        if(strchr(a_ini->d->key[i],':') == NULL)
        {
            nsec++;
        }
    }

    return nsec;
}

/**
 * @brief 获取第n节的名称
 * @param a_ini
 * @return
 */
char *config_ini_get_section_name(config_ini_t *a_ini,int n)
{
    int i ;
    int foundsec ;

    if (a_ini->d==NULL || n<0) return NULL ;
    foundsec=0 ;
    for (i=0 ; i<a_ini->d->size ; i++) {
        if (a_ini->d->key[i]==NULL)
            continue ;
        if (strchr(a_ini->d->key[i], ':')==NULL) {
            foundsec++ ;
            if (foundsec>n)
                break ;
        }
    }
    if (foundsec<=n) {
        return NULL ;
    }
    return a_ini->d->key[i] ;
}

/**
 * @brief 将字典内容保存到文件
 * @param a_ini
 * @param f
 */
void config_ini_save_ini(config_ini_t *a_ini,FILE *f)
{
    int     i ;
    int     nsec ;
    char *  secname ;

    if (a_ini->d==NULL || f==NULL) return ;

    nsec = config_ini_get_section_num(a_ini);
    printf("nsec = %d\n",nsec);
    if (nsec<1) {
        /* No section in file: dump all keys as they are */
        for (i=0 ; i<a_ini->d->size ; i++) {
            if (a_ini->d->key[i]==NULL)
                continue ;
            fprintf(f, "%s = %s\n", a_ini->d->key[i], a_ini->d->val[i]);
        }
        return ;
    }
    for (i=0 ; i<nsec ; i++) {
        secname = config_ini_get_section_name(a_ini, i) ;
        config_ini_savesection_to_file(a_ini, secname, f) ;
    }
    fprintf(f, "\n");
    return ;
}

/**
 * @brief 将一个section节保存到文件中
 * @param a_ini
 * @param sec
 * @param f
 */
void config_ini_savesection_to_file(config_ini_t *a_ini,char *s,FILE *f)
{
    int     j ;
    char    *keym;
    int     secsize ;

    if (a_ini->d==NULL || f==NULL) return ;
    if (! config_ini_find_entry(a_ini, s)) return ;

    fprintf(f, "\n[%s]\n", s);
    secsize = (int)strlen(s) + 2;
    keym = (char *)malloc(secsize);
    snprintf(keym, secsize, "%s:", s);
    for (j=0 ; j<a_ini->d->size ; j++) {
        if (a_ini->d->key[j]==NULL)
            continue ;
        if (!strncmp(a_ini->d->key[j], keym, secsize-1)) {
            fprintf(f,
                    "%-30s = %s\n",
                    a_ini->d->key[j]+secsize-1,
                    a_ini->d->val[j] ? a_ini->d->val[j] : "");
        }
    }
    fprintf(f, "\n");
    free(keym);
    return ;
}

/**
 * @brief 打印到f
 * @param a_ini
 * @param f
 */
void config_ini_dump(config_ini_t *a_ini,FILE *f)
{
    int     i ;

    if (a_ini->d==NULL || f==NULL) return ;
    for (i=0 ; i<a_ini->d->size ; i++) {
        if (a_ini->d->key[i]==NULL)
            continue ;
        if (a_ini->d->val[i]!=NULL) {
            fprintf(f, "[%s]=[%s]\n", a_ini->d->key[i], a_ini->d->val[i]);
        } else {
            fprintf(f, "[%s]=UNDEF\n", a_ini->d->key[i]);
        }
    }
    return ;
}

/**
 * @brief 获取一个节中key的个数
 * @param a_ini
 * @param section
 * @return
 */
int config_ini_get_section_keys_num(config_ini_t *a_ini,const char *s)
{
    int     secsize, nkeys ;
    char    *keym;
    int j ;

    nkeys = 0;

    if (a_ini->d==NULL) return nkeys;
    if (! config_ini_find_entry(a_ini, s)) return nkeys;

    secsize  = (int)strlen(s)+2;
    keym = (char *)malloc(secsize);
    snprintf(keym, secsize, "%s:", s);

    for (j=0 ; j<a_ini->d->size ; j++) {
        if (a_ini->d->key[j]==NULL)
            continue ;
        if (!strncmp(a_ini->d->key[j], keym, secsize-1))
            nkeys++;
    }
    free(keym);
    return nkeys;
}

/**
 * @brief 获取一个节中key列表
 * @param a_ini
 * @param section
 * @return 返回key列表的首地址
 */
char **config_ini_get_section_keys(config_ini_t *a_ini,const char *s)
{
    char **keys;

    int i, j ;
    char    *keym;
    int     secsize, nkeys ;

    keys = NULL;

    if (a_ini->d==NULL) return keys;
    if (! config_ini_find_entry(a_ini, s)) return keys;

    nkeys = config_ini_get_section_keys_num(a_ini, s);

    keys = (char**) malloc(nkeys*sizeof(char*));

    secsize  = (int)strlen(s) + 2;
    keym = (char *)malloc(secsize);
    snprintf(keym, secsize, "%s:", s);

    i = 0;

    for (j=0 ; j<a_ini->d->size ; j++) {
        if (a_ini->d->key[j]==NULL)
            continue ;
        if (!strncmp(a_ini->d->key[j], keym, secsize-1)) {
            keys[i] = a_ini->d->key[j];
            i++;
        }
    }
    free(keym);
    return keys;
}

/**
 * @brief 获取对应key的value值，字符串
 * @param a_ini
 * @param key
 * @param def
 * @return
 *
 * Action!!
 * key的值需添加section  section:key
 */
char *config_ini_getString(config_ini_t *a_ini,const char *key,char *def)
{
    char * lc_key ;
    char * sval ;

    if (a_ini->d==NULL || key==NULL)
        return def ;

    lc_key = xstrdup(key);
    strlwc(lc_key);
    sval = string_dic_get(a_ini->d, lc_key, def);
    if(sval)
        printf("sval = %s\n",sval);
    free(lc_key);
    return sval ;
}

/**
 * @brief 获取value值 int类型
 * @param a_ini
 * @param key
 * @param notfound
 * @return
 *
 * key = section:key
 */
int config_ini_getInt(config_ini_t *a_ini,const char *key,int notfound)
{
    char    *   str ;

    str = config_ini_getString(a_ini, key, (char *)-1);
    if (str==(char *)-1) return notfound ;
    return (int)strtol(str, NULL, 0);
}

/**
 * @brief 获取value值 double类型
 * @param a_ini
 * @param key
 * @param notfound
 * @return
 */
double config_ini_getDouble(config_ini_t *a_ini,const char *key,double notfound)
{
    char    *   str ;

    str = config_ini_getString(a_ini, key, (char *)-1);
    if (str==(char *)-1) return notfound ;
    return atof(str);
}

/**
 * @brief 获取bool
 * @param a_ini
 * @param key
 * @param notfound
 * @return
 */
int config_ini_getBoolean(config_ini_t *a_ini,const char *key,int notfound)
{
    char    *   c ;
    int         ret ;

    c = config_ini_getString(a_ini, key, (char *)-1);
    if (c==(char *)-1) return notfound ;
    if (c[0]=='y' || c[0]=='Y' || c[0]=='1' || c[0]=='t' || c[0]=='T') {
        ret = 1 ;
    } else if (c[0]=='n' || c[0]=='N' || c[0]=='0' || c[0]=='f' || c[0]=='F') {
        ret = 0 ;
    } else {
        ret = notfound ;
    }
    return ret;
}

/**
 * @brief 设置key=value
 * @param a_ini
 * @param entry
 * @param val
 * @return
 */
int config_ini_set(config_ini_t *a_ini,const char *entry,const char *val)
{
    int result = 0;
    char *lc_entry = xstrdup(entry);
    strlwc(lc_entry);
    result = string_dic_set(a_ini->d, lc_entry, val) ;
    free(lc_entry);
    return result;
}

void config_ini_unset(config_ini_t *a_ini,const char *entry)
{
    char* lc_entry = xstrdup(entry);
    strlwc(lc_entry);
    string_dic_unset(a_ini->d, lc_entry);
    free(lc_entry);
}

int config_ini_find_entry(config_ini_t *a_ini,const char *entry)
{
    int found=0 ;
    if (config_ini_getString(a_ini, entry, (char *)-1)!=(char *)-1) {
        found = 1 ;
    }
    return found ;
}
