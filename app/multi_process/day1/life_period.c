#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
	pid_t pid,wait_pid;
	int status;

	pid = fork();

	if (pid==-1)	{
		perror("Cannot create new process");
		exit(1);
	} else 	if (pid==0) {
		printf("child process id: %ld\n", (long) getpid());
		//pause();//wait for signal...
		_exit(1);
	} else {
		do {
			wait_pid=waitpid(pid, &status, WUNTRACED | WCONTINUED);//阻塞，等待pid死亡

			if (wait_pid == -1) {
				perror("cannot using waitpid function");
				exit(1);
			}

			if (WIFEXITED(status))
				printf("child process exits, status=%d\n", WEXITSTATUS(status));

			if(WIFSIGNALED(status))
				printf("child process is killed by signal %d\n", WTERMSIG(status));
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));

		exit(0);
	}
}
