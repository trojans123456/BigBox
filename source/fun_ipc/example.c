#include <unistd.h> /* for pipe */
#include <stdio.h>

int pipe_main()
{
    int fd_w[2];
    int fd_r[2];

    if(pipe(fd_w) < 0)
        return -1;
    if(pipe(fd_r) < 0)
        return -1;

    pid_t pid;
    char buf[20] = "";
    char r_buf[20] = "";

    pid = fork();

    if(pid > 0) /* parent */
    {
        close(fd_w[0]);
        close(fd_r[1]);

        write(fd_w[1],"hello world1\n",13);
        read(fd_r[0],buf,20);
        printf("fd_r = %s\n",buf);
    }
    else if(pid == 0) /* child */
    {
        close(fd_w[1]);
        close(fd_r[0]);

        read(fd_w[0],r_buf,20);
        printf("fd_w = %s\n",r_buf);
        write(fd_r[1],"hello parent\n",14);
    }

    getchar();
}
