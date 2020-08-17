#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv)
{
	pid_t pid;
	char *p;

	p = (char *)mmap(NULL, 4096, PROT_READ | PROT_WRITE,
			 MAP_SHARED | MAP_ANONYMOUS, -1, 0); //vma

	strcpy(p, "XXXXXXXXXXXXXXXXX");

	pid = fork();
	if (pid == -1) {
		exit(-1);
	} else if (pid == 0) { //shared vma
		sprintf(p, "%s", "Hello World");
		munmap(p, 4096);
		_exit(0);
	} else {
		sleep(2);
		printf("%s\n", p);
		munmap(p, 4096);
	}

	return 0;
}
