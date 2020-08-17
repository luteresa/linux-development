#include <signal.h>
#include <stdio.h>
struct two_long { long a, b; } data;
void signal_handler(int signum){
	printf ("%ld, %ld\n", data.a, data.b);
	alarm (1);
}

int main (void){
	static struct two_long zeros = { 0, 0 }, ones = { 1, 1 };
	signal (SIGALRM, signal_handler);
	data = zeros;
	alarm (1);
	while (1)
	{data = zeros; data = ones;}
}

