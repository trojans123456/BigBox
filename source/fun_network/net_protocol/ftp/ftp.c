#include "ftp.h"
#include <sys/types.h>
#include <sys/stat.h>

static const char commandlist[FTP_CMD_MAX][10] =
	{
		"get",
		"put",

		"mget",
		"mput",

		"cd",
		"lcd",

		"mgetwild",
		"mputwild",

		"dir",
		"ldir",

		"ls",
		"lls",

		"mkdir",
		"lmkdir",

		"rget",
		"rput",

		"pwd",
		"lpwd",

		"exit"
	};

static void append_path(struct command* c, char* s)
{
	c->npaths++;
	char** temppaths = (char**) malloc(c->npaths * sizeof(char*));
	if(c->npaths > 1)
		memcpy(temppaths, c->paths, (c->npaths - 1) * sizeof(char*));

	char* temps = (char*) malloc((strlen(s) + 1) * sizeof(char));
	int i;
	for(i = 0; *(temps + i) = *(s + i) == ':' ? ' ' : *(s + i); i++)
		;

	*(temppaths + c->npaths - 1) = temps;

	c->paths = temppaths;
}


struct command* userinputtocommand(char s[LENUSERINPUT])
{
	struct command* cmd = (struct command*) malloc(sizeof(struct command));
	cmd->id = -1;
	cmd->npaths = 0;
	cmd->paths = NULL;
	char* savestate;
	char* token;
	int i, j;
	for(i = 0; ; i++, s = NULL)
	{
		token = strtok_r(s, " \t\n", &savestate);
		if(token == NULL)
			break;
		if(cmd->id == -1)
			for(j = 0; j < FTP_CMD_MAX; j++)
			{
				if(!strcmp(token, commandlist[j]))
				{
					cmd->id = j;
					break;
				}
			}
		else
			append_path(cmd, token);
	}
	if(cmd->id == FTP_CMD_MGET && !strcmp(*cmd->paths, "*"))
		cmd->id = FTP_CMD_MGETWILD;
	else if(cmd->id == FTP_CMD_MPUT && !strcmp(*cmd->paths, "*"))
		cmd->id = FTP_CMD_MPUTWILD;
	if(cmd->id != -1)
		return cmd;
	else
	{
		fprintf(stderr, "\tError parsing command\n");
		return NULL;
	}
}

void set0(struct packet *pckt)
{
    memset(pckt,0x00,sizeof(struct packet));
}

int ntohp(struct packet *pckt,struct packet *dest)
{
    if(pckt && dest)
    {
        memset(dest,0x00,sizeof(struct packet));

        dest->conid = ntohs(pckt->conid);
        dest->type = ntohs(pckt->type);
        dest->comid = ntohs(pckt->comid);
        dest->datalen = ntohs(pckt->datalen);

        memcpy(dest->buffer,pckt->buffer,sizeof(dest->buffer));
    }

    return -1;
};

int htonp(struct packet *pckt,struct packet *dest)
{
    return ntohp(pckt,dest);
}

void printpacket(struct packet *p,int ptype)
{
    if(ptype)
    {
        printf("\t\tHOST PACKET \n");
    }
    else
    {
        printf("\t\tNETWORK PACKET\n");
    }

    printf("\t\tconid = %d\n", p->conid);
	printf("\t\ttype = %d\n", p->type);
	printf("\t\tcomid = %d\n", p->comid);
	printf("\t\tdatalen = %d\n", p->datalen);
	printf("\t\tbuffer = %s\n", p->buffer);

	fflush(stdout);
}

void send_EOT(struct packet *hp, struct packet *data, int sfd)
{
    int x;
	hp->type = FTP_TYPE_EOT;
	//data = htonp(hp);
	htonp(hp,data);
	if((x = send(sfd, data, sizeof(struct packet), 0)) != sizeof(struct packet))
	{
	    fprintf(stderr, "%d\n", x);
	}
}
void send_TERM(struct packet *hp, struct packet *data, int sfd)
{
    int x;
	hp->type = FTP_TYPE_TERM;
	//data = htonp(hp);
	htonp(hp,data);
	if((x = send(sfd, data, sizeof(struct packet), 0)) != sizeof(struct packet))
	{
	     fprintf(stderr, "%d\n", x);
	}
}

void send_file(struct packet *hp, struct packet *data, int sfd, FILE *f)
{
    int x;
	int i = 0, j = 0;
	while(!feof(f))
	{
		memset(hp->buffer, '\0', sizeof(char) * LENBUFFER);
		hp->datalen = fread(hp->buffer, 1, LENBUFFER - 1, f);
		i += hp->datalen;
		//printpacket(hp, HP);
		//data = htonp(hp);
		htonp(hp,data);
		if((x = send(sfd, data, sizeof(struct packet), 0)) != sizeof(struct packet))
		{
		    fprintf(stderr,"%d\n",x);
		    return ;
		}
		j++;
	}
	fprintf(stderr, "\t%d byte(s) read.\n", i);
	fprintf(stderr, "\t%d data packet(s) sent.\n", j);
	fflush(stderr);
}
void receive_file(struct packet *hp, struct packet *data, int sfd, FILE *f)
{
    int x;
	int i = 0, j = 0;
	if((x = recv(sfd, data, sizeof(struct packet), 0)) <= 0)
	{
	    fprintf(stderr,"recv %d\n",x);
	    return ;
	}
	j++;
	//hp = ntohp(data);
	ntohp(data,hp);
	//printpacket(hp, HP);
	while(hp->type == FTP_TYPE_DATA)
	{
		i += fwrite(hp->buffer, 1, hp->datalen, f);
		if((x = recv(sfd, data, sizeof(struct packet), 0)) <= 0)
		{
		    fprintf(stderr," recv %d \n",x);
		    return ;
		}
		j++;
		//hp = ntohp(data);
		ntohp(data,hp);
		//printpacket(hp, HP);
	}
	fprintf(stderr, "\t%d data packet(s) received.\n", --j);	// j decremented because the last packet is EOT.
	fprintf(stderr, "\t%d byte(s) written.\n", i);
	if(hp->type == FTP_TYPE_EOT)
		return;
	else
	{
		fprintf(stderr, "Error occured while downloading remote file.\n");
		exit(2);
	}
	fflush(stderr);
}

void command_pwd(struct packet *chp, struct packet *data, int sfd_client)
{
    int x;
	set0(chp);
	chp->type = FTP_TYPE_REQU;
	chp->conid = -1;
	chp->comid = FTP_CMD_PWD;
	//data = htonp(chp);
	htonp(chp,data);
	if((x = send(sfd_client, data, sizeof(struct packet), 0)) != sizeof(struct packet))
		er("send()", x);
	if((x = recv(sfd_client, data, sizeof(struct packet), 0)) <= 0)
		er("recv()", x);
	//chp = ntohp(data);
	ntohp(data,chp);
	if(chp->type == FTP_TYPE_DATA && chp->comid == FTP_CMD_PWD && strlen(chp->buffer) > 0)
		printf("\t%s\n", chp->buffer);
	else
		fprintf(stderr, "\tError retrieving information.\n");
}

void command_lcd(char *path)
{
    if(chdir(path) == -1)
		fprintf(stderr, "Wrong path : <%s>\n", path);
}

void command_cd(struct packet *chp, struct packet *data, int sfd_client, char *path)
{
    int x;
	set0(chp);
	chp->type = FTP_TYPE_REQU;
	chp->conid = -1;
	chp->comid = FTP_CMD_CD;
	strcpy(chp->buffer, path);
	//data = htonp(chp);
	htonp(chp,data);
	if((x = send(sfd_client, data, sizeof(struct packet), 0)) != sizeof(struct packet))
		er("send()", x);
	if((x = recv(sfd_client, data, sizeof(struct packet), 0)) <= 0)
		er("recv()", x);
	//chp = ntohp(data);
	ntohp(data,chp);
	if(chp->type == FTP_TYPE_INFO && chp->comid == FTP_CMD_CD && !strcmp(chp->buffer, "success"))
		;
	else
		fprintf(stderr, "\tError executing command on the server.\n");
}

void command_lls(char *lpwd)
{
    DIR* d = opendir(lpwd);
	if(!d)
		er("opendir()", (int) 1);
	struct dirent* e;
	while(e = readdir(d))
		printf("\t%s\t%s\n", e->d_type == 4 ? "DIR:" : e->d_type == 8 ? "FILE:" : "UNDEF", e->d_name);
	closedir(d);
}

void command_ls(struct packet *chp, struct packet *data, int sfd_client)
{
    int x;
	set0(chp);
	chp->type = FTP_TYPE_REQU;
	chp->conid = -1;
	chp->comid = FTP_CMD_LS;
	//data = htonp(chp);
	htonp(chp,data);
	if((x = send(sfd_client, data, sizeof(struct packet), 0)) != sizeof(struct packet))
		er("send()", x);
	while(chp->type != FTP_TYPE_EOT)
	{
		if(chp->type == FTP_TYPE_DATA && chp->comid == FTP_CMD_LS && strlen(chp->buffer))
			printf("\t%s\n", chp->buffer);
		/*
		else
			fprintf(stderr, "\tError executing command on the server.\n");
		*/
		if((x = recv(sfd_client, data, sizeof(struct packet), 0)) <= 0)
			er("recv()", x);
		//chp = ntohp(data);
		ntohp(data,chp);
	}
}
void command_get(struct packet *chp, struct packet *data, int sfd_client, char *filename)
{
    FILE* f = fopen(filename, "wb");
	if(!f)
	{
		fprintf(stderr, "File could not be opened for writing. Aborting...\n");
		return;
	}
	int x;
	set0(chp);
	chp->type = FTP_TYPE_REQU;
	chp->conid = -1;
	chp->comid = FTP_CMD_GET;
	strcpy(chp->buffer, filename);
	//data = htonp(chp);
	htonp(chp,data);
	if((x = send(sfd_client, data, sizeof(struct packet), 0)) != sizeof(struct packet))
		er("send()", x);
	if((x = recv(sfd_client, data, sizeof(struct packet), 0)) <= 0)
		er("recv()", x);
	//chp = ntohp(data);
	ntohp(data,chp);
	//printpacket(chp, HP);
	if(chp->type == FTP_TYPE_INFO && chp->comid == FTP_CMD_GET && strlen(chp->buffer))
	{
		printf("\t%s\n", chp->buffer);
		receive_file(chp, data, sfd_client, f);
		fclose(f);
	}
	else
		fprintf(stderr, "Error getting remote file : <%s>\n", filename);
}

void command_put(struct packet *chp, struct packet *data, int sfd_client, char *filename)
{
    FILE* f = fopen(filename, "rb");	// Yo!
	if(!f)
	{
		fprintf(stderr, "File could not be opened for reading. Aborting...\n");
		return;
	}
	int x;
	set0(chp);
	chp->type = FTP_TYPE_REQU;
	chp->conid = -1;
	chp->comid = FTP_CMD_PUT;
	strcpy(chp->buffer, filename);
	//data = htonp(chp);
	htonp(chp,data);
	if((x = send(sfd_client, data, sizeof(struct packet), 0)) != sizeof(struct packet))
		er("send()", x);
	if((x = recv(sfd_client, data, sizeof(struct packet), 0)) <= 0)
		er("recv()", x);
	//chp = ntohp(data);
	ntohp(data,chp);
	//printpacket(chp, HP);
	if(chp->type == FTP_TYPE_INFO && chp->comid == FTP_CMD_PUT && strlen(chp->buffer))
	{
		printf("\t%s\n", chp->buffer);
		chp->type = FTP_TYPE_DATA;
		send_file(chp, data, sfd_client, f);
		fclose(f);
	}
	else
		fprintf(stderr, "Error sending file.\n");

	send_EOT(chp, data, sfd_client);
}


void command_mget(struct packet* chp, struct packet* data, int sfd_client, int n, char** filenames)
{
	int i;
	char* filename;
	for(i = 0; i < n; i++)
	{
		filename = *(filenames + i);
		printf("\tProcessing file %d of %d:\t%s\n", i + 1, n, filename);
		command_get(chp, data, sfd_client, filename);
	}
	if(i != n)
		fprintf(stderr, "Not all files could be downloaded.\n");
}

void command_mput(struct packet* chp, struct packet* data, int sfd_client, int n, char** filenames)
{
	int i;
	char* filename;
	for(i = 0; i < n; i++)
	{
		filename = *(filenames + i);
		printf("\tProcessing file %d of %d:\t%s\n", i + 1, n, filename);
		command_put(chp, data, sfd_client, filename);
	}
	if(i != n)
		fprintf(stderr, "Not all files could be uploaded.\n");
}

void command_mgetwild(struct packet* chp, struct packet* data, int sfd_client)
{
	int x;
	set0(chp);
	chp->type = FTP_TYPE_REQU;
	chp->conid = -1;
	chp->comid = FTP_CMD_LS;
	//data = htonp(chp);
	htonp(chp,data);
	if((x = send(sfd_client, data, sizeof(struct packet), 0)) != sizeof(struct packet))
		er("send()", x);
	struct command* cmd = (struct command*) malloc(sizeof(struct command));
	cmd->id = FTP_CMD_MGETWILD;
	cmd->npaths = 0;
	cmd->paths = NULL;
	while(chp->type != FTP_TYPE_EOT)
	{
		if(chp->type == FTP_TYPE_DATA && chp->comid == FTP_CMD_LS && strlen(chp->buffer))
		if(*chp->buffer == 'F')
			append_path(cmd, chp->buffer + 6);
		if((x = recv(sfd_client, data, sizeof(struct packet), 0)) <= 0)
			er("recv()", x);
		//chp = ntohp(data);
		ntohp(data,chp);
	}
	command_mget(chp, data, sfd_client, cmd->npaths, cmd->paths);
}

void command_mputwild(struct packet* chp, struct packet* data, int sfd_client, char* lpwd)
{
	DIR* d = opendir(lpwd);
	if(!d)
		er("opendir()", (int) 1);
	struct dirent* e;
	struct command* cmd = (struct command*) malloc(sizeof(struct command));
	cmd->id = FTP_CMD_MPUTWILD;
	cmd->npaths = 0;
	cmd->paths = NULL;
	while(e = readdir(d))
		if(e->d_type == 8)
			append_path(cmd, e->d_name);
	closedir(d);
	command_mput(chp, data, sfd_client, cmd->npaths, cmd->paths);
}

void command_rput(struct packet* chp, struct packet* data, int sfd_client)
{
	static char lpwd[LENBUFFER];
	if(!getcwd(lpwd, sizeof lpwd))
		er("getcwd()", 0);
	int x;
	DIR* d = opendir(lpwd);
	if(!d)
		er("opendir()", (int) 1);
	struct dirent* e;
	struct command* cmd = (struct command*) malloc(sizeof(struct command));
	cmd->id = FTP_CMD_RPUT;
	cmd->npaths = 0;
	cmd->paths = NULL;
	while(e = readdir(d))
		if(e->d_type == 8)
			append_path(cmd, e->d_name);
		else if(e->d_type == 4 && strcmp(e->d_name, ".") && strcmp(e->d_name, ".."))
		{
			command_mkdir(chp, data, sfd_client, e->d_name);

			command_cd(chp, data, sfd_client, e->d_name);
			command_lcd(e->d_name);

			command_rput(chp, data, sfd_client);

			command_cd(chp, data, sfd_client, "..");
			command_lcd("..");
		}
	closedir(d);
	command_mput(chp, data, sfd_client, cmd->npaths, cmd->paths);
}

void command_rget(struct packet* chp, struct packet* data, int sfd_client)
{
	char temp[LENBUFFER];
	int x;
	set0(chp);
	chp->type = FTP_TYPE_REQU;
	chp->conid = -1;
	chp->comid = FTP_CMD_RGET;
	//data = htonp(chp);
	htonp(chp,data);
	if((x = send(sfd_client, data, sizeof(struct packet), 0)) != sizeof(struct packet))
		er("send()", x);

	if((x = recv(sfd_client, data, sizeof(struct packet), 0)) <= 0)
		er("recv()", x);
	//chp = ntohp(data);
	ntohp(data,chp);
	//printpacket(chp, HP);
	while(chp->type == FTP_TYPE_REQU)
	{
		if(chp->comid == FTP_CMD_LMKDIR)
		{
			strcpy(temp, chp->buffer);
			command_lmkdir(temp);
		}
		else if(chp->comid == FTP_CMD_LCD)
		{
			strcpy(temp, chp->buffer);
			command_lcd(temp);
		}
		else if(chp->comid == FTP_CMD_GET)
		{
			strcpy(temp, chp->buffer);
			command_get(chp, data, sfd_client, temp);
		}

		if((x = recv(sfd_client, data, sizeof(struct packet), 0)) <= 0)
			er("recv()", x);
		//chp = ntohp(data);
		ntohp(data,chp);
		//printpacket(chp, HP);
	}
	if(chp->type == FTP_TYPE_EOT)
		printf("\tTransmission successfully ended.\n");
	else
		fprintf(stderr, "There was a problem completing the request.\n");
}

void command_mkdir(struct packet* chp, struct packet* data, int sfd_client, char* dirname)
{
	int x;
	set0(chp);
	chp->type = FTP_TYPE_REQU;
	chp->conid = -1;
	chp->comid = FTP_CMD_MKDIR;
	strcpy(chp->buffer, dirname);
	//data = htonp(chp);
	htonp(chp,data);
	if((x = send(sfd_client, data, sizeof(struct packet), 0)) != sizeof(struct packet))
		er("send()", x);
	if((x = recv(sfd_client, data, sizeof(struct packet), 0)) <= 0)
		er("recv()", x);
	//chp = ntohp(data);
	ntohp(data,chp);
	if(chp->type == FTP_TYPE_INFO && chp->comid == FTP_CMD_MKDIR)
	{
		if(!strcmp(chp->buffer, "success"))
			printf("\tCreated directory on server.\n");
		else if(!strcmp(chp->buffer, "already exists"))
			printf("\tDirectory already exitst on server.\n");
	}
	else
		fprintf(stderr, "\tError executing command on the server.\n");
}

void command_lmkdir(char* dirname)
{
	DIR* d = opendir(dirname);
	if(d)
	{
		printf("\tDirectory already exists.\n");
		closedir(d);
	}
	else if(mkdir(dirname, 0777) == -1)
		fprintf(stderr, "Error in creating directory.\n");
	else
		printf("\tCreated directory.\n");
}

