#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void sigint(int sig)
{
	printf("got the SIGINT signal...\n");
}

int main(void)
{
	char buf[100];
	int fd=open("/dev/globalfifo",O_RDONLY);
	
	signal(2,/* sigint*/ SIG_IGN);

	if(fd==-1){
		perror("cannot open the FIFO");
		return -11;
	}

	read(fd, buf, 10);

	return 0;
}
