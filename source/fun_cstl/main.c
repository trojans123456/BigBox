#include <stdio.h>

#include "list.h"
#include "map.h"
#include "alloc_p.h"

int list_main(int argc,char *argv[])
{
    printf("----\n");
    list_t *a_this = list_new(sizeof(int),NULL);
    if(!a_this)
    {
        printf("jsdkljfsdlfjslkdfj\n");
        return 0;
    }

    printf("--------\n");
    int *a = malloc(sizeof(int));
    *a = 100;
    int *b = malloc(sizeof(int));
    *b = 222;
    int *c = malloc(sizeof(int));
    *c = 333;

    list_push_back(a_this,a);

    list_iterator_t it = list_push_back(a_this,b);
    //list_push_back(a_this,c);
    list_insert_before(a_this,it,c);




    int *value = (int *)list_pop_back(a_this);
    if(value)
        printf("value = %d\n",*value);
    else
        printf("fjdskljfsdlk\n");

        value = (int *)list_pop_back(a_this);
    if(value)
        printf("value = %d\n",*value);
    else
        printf("fjdskljfsdlk\n");

        value = (int *)list_pop_back(a_this);
    if(value)
        printf("value = %d\n",*value);
    else
        printf("fjdskljfsdlk\n");

    value = (int *)list_pop_front(a_this);
    if(value)
        printf("value = %d \n",*value);

    getchar();
    return 0;
}

struct test
{
    int a;
    int b;
    int c;
};

bool default_make_pair(char **key,size_t *key_len,void **value,size_t *val_len)
{
    *key_len = strlen(*key) + 1;
    *key = Calloc(1,strlen(*key) + 1);


    *value = Calloc(1,sizeof(struct test));
    *val_len = sizeof(struct test);

    return true;
}
int default_free_pair(char *key,void *value)
{
    if(key)
        Free(key);
    if(value)
        Free(value);
    return 1;
}
int default_compare_key(const char *key1,const char *key2)
{
    return strcmp(key1,key2);
}

static map_ops_t map_ops =
{
  .make_pair = &default_make_pair,
  .free_pair = &default_free_pair,
  .compare_key = &default_compare_key,
};

int map_main(int argc,char *argv[])
{
    map_t *a_this = map_new(&map_ops);


    struct test tt;// = Calloc(1,sizeof(struct test));
    tt.a = 111;
    tt.b = 222;
    tt.c = 333;
    char *a[] = {"test1","test2","test3"};
    map_insert(a_this,a[0],&tt);
    map_insert(a_this,a[1],&tt);
    map_insert(a_this,a[2],&tt);
    printf("size = %d \n",map_size(a_this));
    struct test *ptr = (struct test *)map_find(a_this,"test1");
        if(ptr)
        printf("%d %d %d \n",ptr->a,ptr->b,ptr->c);

    return 0;
}

int main(int argc,char *argv[])
{

    return 0;
}
