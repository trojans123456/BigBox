#ifndef FTP_H
#define FTP_H

#include <stdio.h>

#include <arpa/inet.h>
/*
	for:
		htons()
		htonl()
		ntohs()
		inet_aton()
		inet_ntoa()
*/

#include <netinet/in.h>
/*
	for:
		inet_aton()
		inet_ntoa()
*/

#include <sys/types.h>
/*
	for:
		socket()
		bind()
		recvfrom()
		sendto()
		stat()
*/

#include <sys/socket.h>
/*
	for:
		socket()
		bind()
		recvfrom()
		sendto()
		inet_aton()
		inet_ntoa()
		AF_INET
		SOCK_DGRAM
*/

#include <errno.h>
/*
	for:
		return type of system calls
*/

#include <stdio.h>
/*
	for:
		printf()
		sprintf()
		fflush()
		perror()
*/

#include <stdlib.h>
/*
	for:
		exit()
		malloc()
*/

#include <string.h>
/*
	for:
		memset()
		strlen()
		strcpy()
*/

#include <unistd.h>
/*
	for:
		close()
		sleep()
		stat()
*/

#include <dirent.h>

/*ftp的命令
*/
enum FTP_CMD
{
    FTP_CMD_GET,
    FTP_CMD_PUT,
    FTP_CMD_MGET,
    FTP_CMD_MPUT,
    FTP_CMD_CD,
    FTP_CMD_LCD,
    FTP_CMD_MGETWILD,
    FTP_CMD_MPUTWILD,
    FTP_CMD_DIR,
    FTP_CMD_LDIR,
    FTP_CMD_LS,
    FTP_CMD_LLS,
    FTP_CMD_MKDIR,
    FTP_CMD_RGET,
    FTP_CMD_RPUT,
    FTP_CMD_PWD,
    FTP_CMD_LPWD,
    FTP_CMD_EXIT,
    FTP_CMD_LMKDIR,
    FTP_CMD_MAX
};

enum FTP_TYPE
{
    FTP_TYPE_REQU,
    FTP_TYPE_DONE,
    FTP_TYPE_INFO,
    FTP_TYPE_TERM,
    FTP_TYPE_DATA,
    FTP_TYPE_EOT
};

struct packet
{
    short int conid;
    short int type;
    short int comid;
    short int datalen;
#define LENBUFFER   504
    char buffer[LENBUFFER];
};

void set0(struct packet *pckt);
//np = network packet dest 转换后的
int ntohp(struct packet *pckt,struct packet *dest);

int htonp(struct packet *pckt,struct packet *dest);

#define	er(e, x)					\
	do						\
	{						\
		perror("ERROR IN: " #e "\n");		\
		fprintf(stderr, "%d\n", x);		\
		exit(-1);				\
	}						\
	while(0)

#define TYPE_NP     (0)
#define TYPE_HP     (1)
void printpacket(struct packet *pckt,int ptype);

#define LENUSERINPUT	1024

//提取文件名
#define extract_filename(filepath)  ((strrchr(filepath, '/') != NULL) ? (strrchr(filepath, '/') + 1) : filepath)

void send_EOT(struct packet *hp, struct packet *data, int sfd);
void send_TERM(struct packet *hp, struct packet *data, int sfd);

void send_file(struct packet *hp, struct packet *data, int sfd, FILE *f);
void receive_file(struct packet *hp, struct packet *data, int sfd, FILE *f);

/**** client ****/
struct command
{
    short int id;
    int npaths;
    char **paths;
};

struct command* userinputtocommand(char s[LENUSERINPUT]);

void command_pwd(struct packet *chp, struct packet *data, int sfd_client);
void command_lcd(char *path);
void command_cd(struct packet *chp, struct packet *data, int sfd_client, char *path);
void command_lls(char *lpwd);
void command_ls(struct packet *chp, struct packet *data, int sfd_client);
void command_get(struct packet *chp, struct packet *data, int sfd_client, char *filename);
void command_put(struct packet *chp, struct packet *data, int sfd_client, char *filename);
void command_mget(struct packet*, struct packet*, int, int, char**);
void command_mput(struct packet*, struct packet*, int, int, char**);
void command_mgetwild(struct packet*, struct packet*, int);
void command_mputwild(struct packet*, struct packet*, int, char*);
void command_lmkdir(char*);
void command_mkdir(struct packet*, struct packet*, int, char*);
void command_rget(struct packet*, struct packet*, int);
void command_rput(struct packet*, struct packet*, int);

#endif // FTP_H
