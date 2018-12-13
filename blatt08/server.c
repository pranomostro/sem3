#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <sys/time.h>

#include "list.h"

#define ECHO_PORT 44445
#define HD_PORT 44446

#define MAX_CONN 10

struct connection {
    int sd;
    struct sockaddr_in sin;
    struct timeval timeout;
};

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

    struct sockaddr_in clientAddr;
    unsigned int clientLen;
    int clientSd;

    if ((clientSd = accept(echoSock, (struct sockaddr *) &clientAddr, &clientLen)) < 0) {
        perror("Accept failed");
        close(echoSock);
        exit(-1);
    }

    puts("Accepted");

    int end = recv(clientSd, buffer, sizeof(buffer) - 1, 0);
    buffer[end] = '\0';

    printf("%s\n", buffer);

    send(clientSd, buffer, end + 1, 0);

    close(clientSd);

    close(echoSock);
    return 0;
}