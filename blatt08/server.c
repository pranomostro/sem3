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

#define CONN_TIMEOUT 5
#define SERVER_TIMEOUT 10

#define MAX_CONN 10

enum service {echo, hexdump};

struct connection {
    int sd;
    int timeout;
    enum service service;
};

int nextTimeout (list_t* conns, int serverTimeout) {
    int minTimeout = serverTimeout;
    struct list_elem* le = conns->first;
    while (le != NULL) {
        struct connection* c = (struct connection*) le->data;
        if (c->timeout < minTimeout) {
            minTimeout = c->timeout;
        }
        le = le->next;
    }

    return minTimeout;
}

void updateTimeouts (list_t* conns, int elapsed, int triggeredBy) {
    struct list_elem* le = conns->first;
    while (le != NULL) {
        struct connection* c = (struct connection*) le->data;
        if (c->sd != triggeredBy) {
            c->timeout -= elapsed;
        }
        le = le->next;
    }
}

int cmpSd (const char* d1, const char* d2) {
    struct connection* c1 = (struct connection *) d1;
    struct connection* c2 = (struct connection *) d2;
    return c1->sd - c2->sd;
}

void p (char* e) {
    struct connection* d = (struct connection*) e;
    printf("List SD: %d, TO: %d\n", d->sd, d->timeout);
}

void closeClient(struct pollfd* sds, int i, list_t* conns) {
    close(sds[i].fd);
    printf("Closed %d\n", sds[i].fd);
    
    // Remove from lists
    struct connection c;
    c.sd = sds[i].fd;
    
    sds[i].fd = -1;
    
    struct list_elem* le = list_find(conns, (char*) &c, cmpSd);
    if (le == NULL) {
        printf("Did not find connection in list\n");
        exit(-1);
    }
    free(le->data);                                
    list_remove(conns, le);
}

void addSd (int sd, struct pollfd* sds, int len) {
    int i;
    for (i = 0; i < len; i++) {
        if (sds[i].fd == -1) {
            sds[i].fd = sd;
            return;
        } 
    }

    puts("Can't add new sd");
}

/*
void removeSd (int sd, struct pollfd* sds, int len) {
    for (int i = 0; i < len; i++) {
        if (sds[i].fd == sd) {
            sds[i].fd = -1;
            return;
        } 
    }

    puts("Can't find sd");
}
*/

int main (int argc, char** argv) {
    int echoSock;
    struct sockaddr_in  eSin;

    int serverTimeout = SERVER_TIMEOUT * 1000;

    char buffer[1024];

    list_t* conns = list_init();
    if (conns == NULL) {
       perror("List allocation failed");
       exit(-1);
    }

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
    int i;
    for (i = 0; i < MAX_CONN + 1; i++) {
        sds[i].fd = -1;
        sds[i].events = POLLIN;
    }
    sds[0].fd = echoSock;

    while (1) {
        int pollRet = poll(sds, MAX_CONN + 1, 10000);
        // printf("%d\n", pollRet);
        if (pollRet < 0) {
            perror("Poll failed");
            close(echoSock);
            exit(-1);
        } else if (pollRet == 0) {
            puts("Timeout");
            break;
        }

        for (i = 0; i < MAX_CONN + 1; i++) {
            if (sds[i].fd < 0 || sds[i].revents == 0) {
                continue;
            }
            
            if (sds[i].revents & POLLHUP) {
                closeClient(sds, i, conns);
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

                    printf("[echo]: Accepted %d\n", clientSd);

                    addSd(clientSd, sds, MAX_CONN + 1);

                    struct connection* conn = malloc(sizeof(struct connection));
                    if (conn == NULL) {
                        perror("Can't allocate new connection");
                        exit(-1);
                    }
                    conn->sd = clientSd;
                    conn->service = echo;
                    conn->timeout = CONN_TIMEOUT * 1000;

                    serverTimeout = SERVER_TIMEOUT * 1000;

                    list_append(conns, (char*) conn);
                } else {
                    // Echo
                    int end = recv(sds[i].fd, buffer, sizeof(buffer) - 1, 0);
                    if (end == 0) {
                        closeClient(sds, i, conns);
                    }

                    send(sds[i].fd, buffer, end, 0);
                }
            }
        }
    }

    list_finit(conns);
    free(conns);
    close(echoSock);
    return 0;
}