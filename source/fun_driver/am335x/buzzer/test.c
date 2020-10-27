#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc,char *argv[])
{
	if(argc < 3)
		return -1;

	const char *dev = argv[1];
	int value = atoi(argv[2]);

	int fd = open(dev,O_RDWR);
	if(fd < 0)
	{
		printf("open %s failed\n",dev);
		return -1;
	}

	write(fd,&value,4);

	close(fd);

	return 0;
}
