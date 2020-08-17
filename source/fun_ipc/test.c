#include <stdio.h>

#if 0
/** 管道 */
void pipe_test()
{
    int pipedes[2];
    pid_t pid;

    if(pipe(pipedes) == -1) /*创建管道 */
    {
        return ;
    }

    if(pid = fork() == -1) /* 创建新进程 */
    {
        return ;
    }
    else if(pid == 0) /* child */
    {
        if(write(pipedes[1],"xx",2) == -1) /* write to pipe*/
        {
            return ;
        }
    }
    else if(pid > 0)
    {
        /* parent */
        char buffer[14];
        read(pipedes[0],buffer,14);
    }
}

/** 有名管道 */
void mkfifo_test()
{
    mkfifo("/tmp/cmd_pipe",S_IFIFO | 0666);

    int fd = open("tmp/cmd_pipe",);
    write(fd,"xx",2);
    char buffer[128];
    read(fd,buffer,2);

    mknod(FIFO_FILE,S_IFIFO|0666,0);

}

void handler(int signum)
{

}

void signal_test()
{

    signal(SIGINT,handler); /*不够健壮 推荐 sigaction */
}

#endif

int main(int argc,char *argv[])
{

    return 0;
}
