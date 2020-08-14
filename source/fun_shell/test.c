#include <stdio.h>

#include "sys_exec.h"

int main(int argc,char *argv[])
{
    char buffer[128] = "";
    int size = 128;
    sys_shell_exec2("ls -lh",buffer,&size);
    printf("buffer = %s\n",buffer);
    return 0;
}
