#include <signal.h>
#include <stdio.h>
struct two_int { long a, b; } data;
void signal_handler(int signum){
	printf ("%ld, %ld\n", data.a, data.b);
	alarm (1);
}
int main (void){
	sigset_t   bset, oset;
	static struct two_int zeros = { 0, 0 }, ones = { 1, 1 };
	signal (SIGALRM, signal_handler);
	data = zeros;
	alarm (1);
	sigemptyset(&bset);
	sigaddset(&bset, SIGALRM);

	while (1) {
		if (sigprocmask(SIG_BLOCK, &bset, &oset) != 0)
			printf("!! Set mask failed\n");
		data = zeros;
		if (sigprocmask(SIG_SETMASK, &oset, NULL) != 0)
			printf("!! Set mask failed\n");

		if (sigprocmask(SIG_BLOCK, &bset, &oset) != 0)
			printf("!! Set mask failed\n");
		data = ones;
		if (sigprocmask(SIG_SETMASK, &oset, NULL) != 0)
			printf("!! Set mask failed\n");
	}
}

