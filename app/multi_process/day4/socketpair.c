#include <stdio.h> 
#include <string.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <errno.h> 
#include <sys/socket.h> 
#include <stdlib.h> 

const char* str = "Hello World";

int main(int argc, char* argv[]){
    char buf[128] = {0};
    int fd[2]; 
    pid_t pid; 

    if(socketpair(AF_UNIX, SOCK_STREAM, 0, fd) == -1 ) { 
        return EXIT_FAILURE; 
    } 

    pid = fork();
    if(pid < 0) {
        return EXIT_FAILURE;
    } else if(pid > 0) {
        close(fd[1]);

        write(fd[0], str, strlen(str));
	read(fd[0], buf, sizeof(buf));
	printf("parent received: %s\n", buf);
    } else if(pid == 0) {
        close(fd[0]);

        read(fd[1], buf, sizeof(buf));  
	write(fd[1], str, strlen(str));
        printf("child received: %s\n",buf);
    }

    while(1);
} 
