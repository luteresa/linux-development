#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>

struct thread_param
{
	char info;
	int num;
};

#if 0
void segvSignal(int sig)
{
	printf("thread pid:%d, tid:%lu\n",getpid(), pthread_self());
}
#endif

void sighandler1(int sig)
{
	printf("%s got the signal...\n", __func__);
}

void sighandler2(int sig)
{
	printf("%s got the signal...\n", __func__);
}

void* thread_fun(void* param)
{
	struct thread_param* p;
	
	p=(struct thread_param*)param;
	int i;

	if (signal(SIGINT,sighandler2) == SIG_ERR)
		return -1;

	printf("thread pid:%d, tid:%lu\n",getpid(), pthread_self());
	for(i=0;i<p->num;i++){

#if  1 /* change to #if 1 for debugging high cpu-loading issues */
		while(1);
#else
		sleep(1);
#endif
		printf("%i: %c\n",i,p->info);
	}
		
	return NULL;
}

int main(void)
{
	pthread_t tid1,tid2;
	struct thread_param info1;
	struct thread_param info2;
	int ret;

	if (signal(SIGINT,sighandler1) == SIG_ERR)
		return -1;

	info1.info='T';
	info1.num=2000000;

	printf("main pid:%d, tid:%lu\n",getpid(), pthread_self());
	
	ret=pthread_create(&tid1,NULL,thread_fun,&info1);
	if(ret==-1){
		perror("cannot create new thread");
		return 1;
	}

	info2.info='S';
	info2.num=300000;
	
	ret=pthread_create(&tid2,NULL,thread_fun,&info2);
	if(ret==-1){
		perror("cannot create new thread");
		return 1;
	}

	sleep(1);

	pthread_kill(pthread_self(), SIGINT);

	if(pthread_join(tid1,NULL)!=0){
		perror("call pthread_join function fail");
		return 1;
	}

	if(pthread_join(tid2,NULL)!=0){
		perror("call pthread_join function fail");
		return 1;
	}

	return 0;
}	
