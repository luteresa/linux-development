#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>

void* thread_fun(void* param)
{
	printf("thread pid:%d, tid:%lu\n",getpid(), pthread_self());
	while(1);
}

int main(void)
{
	pthread_t tid1,tid2,tid3,tid4;

	printf("main pid:%d, tid:%lu\n",getpid(), pthread_self());
	
	pthread_create(&tid1,NULL,thread_fun,NULL);
	pthread_create(&tid2,NULL,thread_fun,NULL);
	pthread_create(&tid3,NULL,thread_fun,NULL);
	pthread_create(&tid4,NULL,thread_fun,NULL);

	pthread_join(tid1,NULL);
	pthread_join(tid2,NULL);
	pthread_join(tid3,NULL);
	pthread_join(tid4,NULL);

	return 0;
}	
