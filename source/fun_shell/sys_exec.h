#ifndef __SYS_EXEC_H
#define __SYS_EXEC_H


#ifdef __cplusplus
extern "C" {
#endif

#define SYSTEM(x)   system((x))

/**
 * @brief 执行shell命令
 * @param cmd
 * @param out 命令输出缓存
 * @param putsize
 * @return
 */
int sys_shell_exec(const char *cmd,char *output,int *putsize);


typedef struct files_t files_t;

files_t *popen2(const char *command);

void pwrites(files_t *h,const char *cmd);

const char *preads(void *buffer,int buffer_len,files_t *h);

int pclose2(files_t *h);

int sys_shell_exec2(const char *cmd, char *output, int *putsize);

#ifdef __cplusplus
}
#endif

#endif
