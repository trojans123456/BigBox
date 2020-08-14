#include <stdio.h>
#include "thread.h"
#include "resalloc.h"

void *test_func(void *args)
{
    thread_t *th = (thread_t *)args;

    printf("-----------\n");
    while(1)
    {
        printf("xxxx\n");
        ThreadSleep(1000);
    }

}

int main(int argc,char *argv[])
{
    thread_t *th;
    th = ThreadCreate("test",test_func,th);
    if(th)
    {
        printf("--%d \n",isRunning(th) ? 1 : 0);
        ThreadStart(th);

        printf("%d \n",isRunning(th) ? 1 : 0);
        while(1) ;
    }
    return 0;
}
