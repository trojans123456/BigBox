#include <stdio.h>

#include "sys_exec.h"

int sys_shell_exec(const char *cmd,char *output,int *putsize)
{

    if(!cmd)
    {
        return 0;
    }

    FILE *fp = popen(cmd,"r");
    if(!fp)
    {
        return 0;
    }

    if(output)
    {
        int default_len = *putsize;
        int real_len = 0,sum_len = 0;

        while(1)
        {
            real_len = fread(output + sum_len,default_len - sum_len,sizeof(char),fp);
            if(real_len < 0)
            {
                *putsize = real_len;
                return 0;
            }
            else if(real_len == 0)
            {
                *putsize = sum_len;
                break;
            }
            if(real_len == *putsize)
                break;
            sum_len += real_len;
        }
    }
    pclose(fp);

    return 1;
}


#include<errno.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<unistd.h>

struct files_t
{
    FILE *in;
    FILE *out;
};
struct files_chain_t
{
    struct files_t files;
    pid_t pid;
    struct files_chain_t *next;
};
typedef struct files_chain_t files_chain_t;
static files_chain_t *files_chain;

#define CLEANUP_PIPE(pipe) close((pipe)[0]); close((pipe)[1])

static int _do_popen2(files_chain_t *link, const char *command)
{
    int child_in[2];
    int child_out[2];
    if (0 != pipe(child_in)) {
        return -1;
    }
    if (0 != pipe(child_out)) {
        CLEANUP_PIPE(child_in);
        return -1;
    }

    pid_t cpid = link->pid = fork();
    if (0 > cpid) {
       CLEANUP_PIPE(child_in);
       CLEANUP_PIPE(child_out);
       return -1;
    }
    if (0 == cpid) {
        if (0 > dup2(child_in[0], 0) || 0 > dup2(child_out[1], 1)) {
            _Exit(127);
        }
        CLEANUP_PIPE(child_in);
        CLEANUP_PIPE(child_out);
        files_chain_t *p;
        for ( p = files_chain; p; p = p->next) {
            int fd_in = fileno(p->files.in);
            if (fd_in != 0) {
                close(fd_in);
            }
            int fd_out = fileno(p->files.out);
            if (fd_out != 1) {
                close(fd_out);
            }
        }

        execl("/bin/sh", "sh", "-c", command, (char *) NULL);
        _Exit(127);
    }

    close(child_in[0]);
    close(child_out[1]);
    link->files.in = fdopen(child_in[1], "w");
    link->files.out = fdopen(child_out[0], "r");
    return 0;
}

/**
 * NAME
 *     popen2 -- bidirectional popen()
 *
 * DESCRIPTION
 *     popen2(const char *command) opens two pipes, forks a child process,
 *     then binds the pipes to its stdin and stdout and execve shell to
 *     execute given command.
 *
 * RETURN VALUES:
 *     On success it returns a pointer to the struct with two fields
 *     { FILE *in; FILE *out; }. The struct should be released via pclose2()
 *     call. On failure returns NULL, check errno for more informaion about
 *     the error.
 */
files_t *popen2(const char *command)
{
    files_chain_t *link = (files_chain_t *) malloc(sizeof (files_chain_t));
    if (NULL == link) {
        return NULL;
    }

    if (0 > _do_popen2(link, command)) {
        free(link);
        return NULL;
    }

    link->next = files_chain;
    files_chain = link;
    return (files_t *) link;
}

int pclose2(files_t *fp) {
    files_chain_t **p = &files_chain;
    int found = 0;
    while (*p) {
        if (*p == (files_chain_t *) fp) {
            *p = (*p)->next;
            found = 1;
            break;
        }
        p = &(*p)->next;
    }

    if (!found) {
        return -1;
    }
    if (0 > fclose(fp->in) || 0 > fclose(fp->out)) {
        free((files_chain_t *) fp);
        return -1;
    }

    int status = -1;
    pid_t wait_pid;
    do {
        wait_pid = waitpid(((files_chain_t *) fp)->pid, &status, 0);
    } while (-1 == wait_pid && EINTR == errno);

    free((files_chain_t *) fp);

    if (wait_pid == -1) {
        return -1;
    }
    return status;
}

void pwrites(files_t *h,const char *cmd)
{
    if(h)
    {
        fputs(cmd,h->in);
        fflush(h->in);
    }
}

const char *preads(void *buffer,int buffer_len,files_t *h)
{
    if(h && buffer)
    {
        return fgets(buffer,buffer_len,h->out);
    }
    return NULL;
}

int sys_shell_exec2(const char *cmd, char *output, int *putsize)
{
    files_t *fp;
    fp = popen2(cmd);
    if(!fp)
        return -1;

    while(preads(output,*putsize,fp) != NULL)
    {

    }

    pclose2(fp);

    return 0;
}
