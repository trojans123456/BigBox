#include <stdio.h>
#include "ftp/e_ftp.h"


#define	PORTSERVER	6800
#define IPSERVER	"192.168.1.18"
#define	ID		"CLIENT=> "

int ftp_client(int argc, char* argv[])
{
	//BEGIN: initialization
	struct sockaddr_in sin_server;
	int sfd_client, x;
	size_t size_sockaddr = sizeof(struct sockaddr);
	int size_packet = sizeof(struct packet);
	short int connection_id;
	struct packet* chp = (struct packet*) malloc(size_packet);		// client host packet
	set0(chp);
	struct packet* data = (struct packet *)malloc(sizeof(struct packet));							// network packet

	if((x = sfd_client = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		er("socket()", x);

	memset((char*) &sin_server, 0, sizeof(struct sockaddr_in));
	sin_server.sin_family = AF_INET;
	sin_server.sin_addr.s_addr = inet_addr(IPSERVER);
	sin_server.sin_port = htons(PORTSERVER);

	if((x = connect(sfd_client, (struct sockaddr*) &sin_server, size_sockaddr)) < 0)
		er("connect()", x);

	printf(ID "FTP Client started up. Attempting communication with server @ %s:%d...\n\n", IPSERVER, PORTSERVER);
	//END: initialization


	struct command* cmd;
	char lpwd[LENBUFFER], pwd[LENBUFFER];
	char userinput[LENUSERINPUT];
	while(1)
	{
		printf("\t> ");
		fgets(userinput, LENUSERINPUT, stdin);	// in order to give \
					a filename with spaces, put ':' \
					instead of ' '. If a command needs \
					x paths, and y (y > x) paths are \
					provided, y - x paths will be \
					ignored.
		cmd = userinputtocommand(userinput);
		if(!cmd)
			continue;
		//printcommand(cmd);
		switch(cmd->id)
		{
			case FTP_CMD_GET:
				if(cmd->npaths)
					command_get(chp, data, sfd_client, *cmd->paths);
				else
					fprintf(stderr, "No path to file given.\n");
				break;
			case FTP_CMD_PUT:
				if(cmd->npaths)
					command_put(chp, data, sfd_client, *cmd->paths);
				else
					fprintf(stderr, "No path to file given.\n");
				break;
			case FTP_CMD_MGET:
				if(cmd->npaths)
					command_mget(chp, data, sfd_client, cmd->npaths, cmd->paths);
				else
					fprintf(stderr, "No path to file given.\n");
				break;
			case FTP_CMD_MPUT:
				if(cmd->npaths)
					command_mput(chp, data, sfd_client, cmd->npaths, cmd->paths);
				else
					fprintf(stderr, "No path to file given.\n");
				break;
			case FTP_CMD_MGETWILD:
				command_mgetwild(chp, data, sfd_client);
				break;
			case FTP_CMD_MPUTWILD:
				if(!getcwd(lpwd, sizeof lpwd))
					er("getcwd()", 0);
				command_mputwild(chp, data, sfd_client, lpwd);
				break;
			case FTP_CMD_CD:
				if(cmd->npaths)
					command_cd(chp, data, sfd_client, *cmd->paths);
				else
					fprintf(stderr, "No path given.\n");
				break;
			case FTP_CMD_LCD:
				if(cmd->npaths)
					command_lcd(*cmd->paths);
				else
					fprintf(stderr, "No path given.\n");
				break;
			case FTP_CMD_PWD:
				command_pwd(chp, data, sfd_client);
				break;
			case FTP_CMD_LPWD:
				if(!getcwd(lpwd, sizeof lpwd))
					er("getcwd()", 0);
				printf("\t%s\n", lpwd);
				break;
			case FTP_CMD_DIR:
			case FTP_CMD_LS:
				command_ls(chp, data, sfd_client);
				break;
			case FTP_CMD_LDIR:
			case FTP_CMD_LLS:
				if(!getcwd(lpwd, sizeof lpwd))
					er("getcwd()", 0);
				command_lls(lpwd);
				break;
			case FTP_CMD_MKDIR:
				if(cmd->npaths)
					command_mkdir(chp, data, sfd_client, *cmd->paths);
				else
					fprintf(stderr, "No path to directory given.\n");
				break;
			case FTP_CMD_LMKDIR:
				if(cmd->npaths)
					command_lmkdir(*cmd->paths);
				else
					fprintf(stderr, "No path to directory given.\n");
				break;
			case FTP_CMD_RGET:
				if(!getcwd(lpwd, sizeof lpwd))
					er("getcwd()", 0);
				command_rget(chp, data, sfd_client);
				if((x = chdir(lpwd)) == -1)
					fprintf(stderr, "Wrong path.\n");
				break;
			case FTP_CMD_RPUT:
				if(!getcwd(lpwd, sizeof lpwd))
					er("getcwd()", 0);
				command_rput(chp, data, sfd_client);
				if((x = chdir(lpwd)) == -1)
					fprintf(stderr, "Wrong path.\n");
				break;
			case FTP_CMD_EXIT:
				goto outside_client_command_loop;
			default:
				// display error
				break;
		}
	}
	outside_client_command_loop:



	close(sfd_client);
	printf(ID "Done.\n");
	fflush(stdout);

	return 0;
}
