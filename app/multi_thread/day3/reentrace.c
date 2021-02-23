#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>
#include <sys/types.h>

char *strtoupper(char *string, char *buffer)
{
                int index;

                for (index = 0; string[index]; index++)
                                buffer[index] = toupper(string[index]);
                buffer[index] = 0; 

                return buffer;
}

void *thread_fun(void *param)
{
	while (1) {
		char buff[1000];
		usleep(100);
		strtoupper((char *)param, buff);
		printf("%s\n", buff);
	}
}

int main(void)
{
	pthread_t tid1, tid2;
	int ret;

	printf("main pid:%d, tid:%lu\n", getpid(), pthread_self());

	ret = pthread_create(&tid1, NULL, thread_fun, "hello world");
	if (ret == -1) {
		perror("cannot create new thread");
		return 1;
	}

	ret = pthread_create(&tid2, NULL, thread_fun, "world hello");
	if (ret == -1) {
		perror("cannot create new thread");
		return 1;
	}

	if (pthread_join(tid1, NULL) != 0) {
		perror("call pthread_join function fail");
		return 1;
	}

	if (pthread_join(tid2, NULL) != 0) {
		perror("call pthread_join function fail");
		return 1;
	}

	return 0;
}
