#include <limits.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

static char *p;//bss->0

static void *thread_routine(void *arg)
{
	char buf[10] = { 0 };
	p = buf;

	while (1) {
		printf("buf is %s\n", buf);
		sleep(1);
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	pthread_t thread_id1;
	pthread_attr_t thread_attr;
	size_t stack_size, guard_size;
	int status;

	status = pthread_attr_init(&thread_attr);
	if (status != 0)
		exit(-1);

	status = pthread_create(&thread_id1, &thread_attr, thread_routine, NULL);
	if (status != 0)
		exit(-1);

	int i = 0;
	while (1) {
		if (p) {
			if (i++ % 2 == 0)
				strcpy(p, "helloworld1");
			else
				strcpy(p, "world");
		}
		sleep(4);
	}

	pthread_join(thread_id1, NULL);

	return 0;
}
