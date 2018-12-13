#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/poll.h>

#include "list.h"

#define ECHO_PORT 44445
#define HD_PORT 44446

#define MAX_CONN 10

struct connection {
    int sd;
    struct sockaddr_in sin;
    struct timeval timeout;
};

void closeClient(struct pollfd* sds, int i, list_t* conns) {
    close(sds[i].fd);
    printf("Closed %d\n", sds[i].fd);
    sds[i].fd = -1;
}

void addSd (int sd, struct pollfd* sds, int len) {
    for (int i = 0; i < len; i++) {
        if (sds[i].fd == -1) {
            sds[i].fd = sd;
            return;
        } 
    }

    puts("Can't add new sd");
}

void removeSd (int sd, struct pollfd* sds, int len) {
    for (int i = 0; i < len; i++) {
        if (sds[i].fd == sd) {
            sds[i].fd = -1;
            return;
        } 
    }

    puts("Can't find sd");
}

int main (int argc, char** argv) {
    int echoSock;
    struct sockaddr_in  eSin;

    char buffer[1024];

    if ((echoSock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(-1);
    }

    puts("Created echo");

    // Fill eSin
    eSin.sin_family = AF_INET;
    eSin.sin_port = htons(ECHO_PORT);
    eSin.sin_addr.s_addr = INADDR_ANY;

    if (bind(echoSock, (struct sockaddr *) &eSin, sizeof(eSin)) != 0) {
        perror("Bind failed");
        close(echoSock);
        exit(-1);
    }

    puts("Bound");

    if (listen(echoSock, MAX_CONN) != 0) {
        perror("Listen failed");
        close(echoSock);
        exit(-1);
    }

    puts("Listening...");

    struct pollfd sds[MAX_CONN + 1];
    // Init pollfds
    for (int i; i < MAX_CONN + 1; i++) {
        sds[i].fd = -1;
        sds[i].events = POLLIN;
    }
    sds[0].fd = echoSock;
    //printf("%d\n", echoSock);

    while (1) {
        int pollRet = poll(sds, MAX_CONN + 1, 10000);
        printf("%d\n", pollRet);
        if (pollRet < 0) {
            perror("Poll failed");
            close(echoSock);
            exit(-1);
        } else if (pollRet == 0) {
            puts("Timeout");
            close(echoSock);
            exit(-1);
        }

        for (int i = 0; i < MAX_CONN + 1; i++) {
            if (sds[i].fd < 0 || sds[i].revents == 0) {
                continue;
            }

            printf("FD:%d I:%d rev:%x hup:%x\n", sds[i].fd, i, sds[i].revents, POLLHUP);
            

            if (sds[i].revents & POLLHUP) {
                closeClient(sds, i, NULL);
            } else if (sds[i].revents & POLLIN) {
                if (sds[i].fd == echoSock) {
                    // Accept
                    struct sockaddr_in clientAddr;
                    unsigned int clientLen;
                    int clientSd;

                    if ((clientSd = accept(echoSock, (struct sockaddr *) &clientAddr, &clientLen)) < 0) {
                        perror("Accept failed");
                        close(echoSock);
                        exit(-1);
                    }

                    puts("Accepted");

                    printf("%d\n", clientSd);

                    addSd(clientSd, sds, MAX_CONN + 1);
                } else {
                    puts("echo");
                    // Echo
                    int end = recv(sds[i].fd, buffer, sizeof(buffer) - 1, 0);
                    if (end == 0) {
                        closeClient(sds, i, NULL);
                    }

                    send(sds[i].fd, buffer, end, 0);
                }
            }
        }
    }

    close(echoSock);
    return 0;
}