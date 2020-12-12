#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int i=0; 
	while(1){
		sleep(1);
		printf("hello damon index:%d\n",i++);
	}
}
