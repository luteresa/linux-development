#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sanitizer/lsan_interface.h>

void sighandler(int signo)
{
    __lsan_do_leak_check();
    // raise the signal again to crash process
    raise(signo); //给当前进程发送signo信号
}

__attribute__ ((constructor))
void ctor()
{
	printf("enter ctor...\n");
    int sigs[] = {
        SIGILL, SIGFPE, SIGABRT, SIGBUS,
        SIGSEGV, SIGHUP, SIGINT, SIGQUIT,
        SIGTERM
    };
    int i;
    struct sigaction sa;
    sa.sa_handler = sighandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESETHAND;
    for(i = 0; i < sizeof(sigs)/sizeof(sigs[0]); ++i) {
        if (sigaction(sigs[i], &sa, NULL) == -1) {
            perror("Could not set signal handler");
        }
    }
}

__attribute__ ((destructor))
void dector()
{
	printf("deconstructor\n");
}
