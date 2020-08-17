#include <signal.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

static jmp_buf env;

void segvSignal(int sig)
{
	longjmp(env, 1);
}

int main(int argc, char **argv)
{
	int r = setjmp(env);
	if (r == 0) {
		signal(SIGSEGV, segvSignal);

		printf("making segment fault\n");
		int *p = NULL;
		*p = 0;
	} else {
		printf("after segment fault\n");
	}

	return 0;
}
