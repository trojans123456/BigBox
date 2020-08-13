#include <stdio.h>
#include <string.h>

#include "string_dic_p.h"
#include "ini_config1.h"
#include "ini_config2.h"

int string_dic_test(int argc,char *argv[])
{
    string_dic_t  *d ;
    char    *   val ;
    int         i ;
    char        cval[90] ;
    char value[32];

    /* Allocate dictionary */
    printf("allocating...\n");
    d = string_dic_new(0);

    /* Set values in dictionary */
    printf("setting %d values...\n", 20000);
    for (i=0 ; i<20000 ; i++) {
        sprintf(cval, "%04d", i);
        sprintf(value,"salut%04d",i);
        string_dic_set(d, cval, value);
    }
    printf("getting %d values...\n", 20000);
    for (i=0 ; i<20000 ; i++) {
        sprintf(cval, "%04d", i);
        val = string_dic_get(d, cval, "def");
        if (!strcmp(val,"def")) {
            printf("cannot get value for key [%s]\n", cval);
        }
        else
        {
            printf("val = %s\n",val);
        }
    }
    printf("unsetting %d values...\n", 20000);
    for (i=0 ; i<20000 ; i++) {
        sprintf(cval, "%04d", i);
        string_dic_unset(d, cval);
    }
    if (string_dic_get_entries(d) != 0) {
        printf("error deleting values\n");
    }

    printf("deallocating...\n");
    string_dic_del(d);

    return 0;
}

int ini_config1_main(int argc,char *argv[])
{
    config_ini_t *conf = config_ini_new();
    if(!conf)
    {
        printf("config ini new failed\n");
        return 0;
    }

    config_ini_set(conf,"wine","");
    config_ini_set(conf,"wine:year","2020");
    config_ini_set(conf,"wine:month","08");
    config_ini_set(conf,"wine:day","13");


    char *val = config_ini_getString(conf,"wine:year","xx");
    if(val)
    printf("--%s\n",val);

    config_ini_save(conf,"ini_test");

    config_ini_free(conf);

    config_ini_t *conf1 = config_init_load("ini_test");
    if(!conf1)
        return 0;

    char *value = config_ini_getString(conf1,"wine:year","xx");
    if(value)
        printf("value = %s\n",value);

    config_ini_free(conf1);

    return 0;
}

int main(int argc,char *argv[])
{
    ini_t *ini = ini_load("ini_test");
    if(ini)
    {
        const char *val = ini_get(ini,"wine","year");
        if(val)
            printf("val = %s\n",val);
    }
}
