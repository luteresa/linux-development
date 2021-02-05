#include <stdio.h>
#include <sched.h>

int data = 10;

int child_process()
{
	printf("Child process %d, data %d\n",getpid(),data);
	data = 20;
	printf("Child process %d, data %d\n",getpid(),data);
	_exit(0); //kernel/exit.c/mm_release(tsk,mm),释放vfork_done
	//while(1);
}

int main(int argc, char* argv[])
{
	if(vfork()==0) {
		child_process();	
	}
	else{//fork后阻塞，当mm不再被共享时，继续运行
		sleep(1);
		printf("Parent process %d, data %d\n",getpid(), data);
	}
}
