#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

char * server_filename = "/tmp/socket-server";

int main(void)
{
    int s;
    struct sockaddr_un srv_un = {0};

    if ((s = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
        perror("socket server");
        exit(1);
    }

    srv_un.sun_family = AF_UNIX;
    strncpy(srv_un.sun_path, server_filename, sizeof(srv_un.sun_path));
    /*If you leave the file behind when you're finished, or perhaps crash after binding, the next bind will fail
    / with "address in use". Which just means, the file is already there.*/
    unlink(srv_un.sun_path);

    if (bind(s, (struct sockaddr *)&srv_un, sizeof(srv_un)) == -1) {
        perror("bind server");
        exit(1);
    }

    for(;;) {

        char buf[1024] = {0};
        read(s, buf, sizeof(buf));
        printf("RECEIVED: %s", buf);

    }

    close(s);

    return 0;
}
