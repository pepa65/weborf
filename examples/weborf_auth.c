// weborf_auth.c
// Weborf copyright 2009 (GPL3+) Salvo "LtWorf" Tomaselli <tiposchi@tiscali.it>

//Example of authentication program

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCK_PATH "/tmp/weborf_auth.socket"
#define SIZE 1024

int main(void) {
    int s,s2, t, len;
    struct sockaddr_un local, remote;
    char str[SIZE];

    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, SOCK_PATH);
    unlink(SOCK_PATH);
    len = strlen(local.sun_path) + sizeof(local.sun_family);

    if (bind(s, (struct sockaddr *)&local, len) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(s, 5) == -1) {
        perror("listen");
        exit(1);
    }

    for (;;) {
        t = sizeof(remote);
        if ((s2 = accept(s, (struct sockaddr *)&remote, &t)) == -1) {
            perror("accept");
            exit(1);
        }

        t = recv(s2, str, SIZE, 0);
        str[t] = '\0';

        if (str[1]=='f' && str[2]=='i' && str[3]=='l' && str[4]=='m')  {
            if (strstr(str,"::ffff:10.0.")==NULL) {
                send(s2,"c", 1,0);
            }

        }

        close(s2);
    }

    return 0;
}
