#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

//int atexit(void (*function)(void));
void exit1print(void)
{
	printf("hello exit1\n");
}

void exit2print(void)
{
	printf("hello exit2\n");
}

int main(int argc, char *argv[])
{
	atexit(exit1print);
	atexit(exit2print);
	printf("hello main enter");
	//while(1); //用信号杀死，调用_exit()退出，不调用钩子函数
	_exit(1); //非正常退出，钩子函数不调用, printf无回车，不打印
	return 1;   //正常退出，调用钩子函数,exit()
}
