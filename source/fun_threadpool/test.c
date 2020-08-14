#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "threadpool.h"

static void *write_file(void *arg)
{
    char path[128] = "";
    int file_index = *((int*)arg);
    printf("file_index = %d \n",file_index);
    sprintf(path,"test_%d.log",file_index);

    FILE *file_ptr = fopen(path,"w+");
    if(file_ptr)
    {
        char *str = "hello, i am test program\n";
        int i = 0;
        for(i = 0;i < 100;i++)
        {
            fwrite(str,sizeof(char),strlen(str),file_ptr);
        }

        fclose(file_ptr);
    }
    else
    {
        printf("open failed\n");
    }
    return NULL;
}

int main(int argc,char *argv[])
{
    threadpool_t *pool = threadpool_create(8,65535);
    if(!pool)
    {
        printf("thread pool faild\n");
        return 0;
    }
    int *file_index = (int *)malloc(sizeof(int) * 8);
    int i = 0;
    int ret;
    for(i = 0;i < 8;i++)
    {
        file_index[i] = i;
    }

    for(i = 0;i < 8;i++)
    {
        if((ret = threadpool_add(pool,write_file,(void *)&file_index[i])) != 0)
        {
            printf("add task faild %d\n",ret);
        }
    }

    for(;;) ;

}
