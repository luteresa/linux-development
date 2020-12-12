#include <stdio.h>
#include <unistd.h>
#include <string.h>
int main()
{
	int fd[2];

	int ret = pipe(fd);
	if (ret < 0) {
		perror("pipe\n");
	}
	pid_t id = fork();
	if (id < 0) {
		perror("fork\n");
	} else if (id != 0) {
		close(fd[0]);

		char *buf = "hello world\n";
		write(fd[1], buf, strlen(buf) + 1);
		//while (1) ;
	} else {
		close(fd[1]);

		char buf[100];
		read(fd[0], buf, 100);
		printf("%s\n", buf);
		while (1) ;
	}

	return 0;
}
