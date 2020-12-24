#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void sighandler(int sig)
{
	printf("got the signal...\n");
}

int main(void)
{
	//if(signal(SIGINT, /*sighandler*/SIG_IGN)==SIG_ERR){
	if(signal(SIGINT, sighandler)==SIG_ERR){
		perror("cannot reset the SIGINT signal handler");
		return 1;
	}

	while(1);
	return 0;
}
