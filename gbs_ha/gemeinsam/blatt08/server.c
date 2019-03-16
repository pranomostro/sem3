#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/poll.h>
#include <string.h>

#include "list.h"

#define ECHO_PORT 44445
#define HD_PORT 44446

#define CONN_TIMEOUT 5000
#define SERVER_TIMEOUT 10000

#define MAX_CONN 10
#define SERVICE_COUNT 2

extern void hexdump(int sd, char* buffer, int length);

enum service {echo, hexdump_s};

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

void updateTimeouts (list_t* conns, int elapsed) {
    struct list_elem* le = conns->first;
    while (le != NULL) {
        struct connection* c = (struct connection*) le->data;
        c->timeout -= elapsed;
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

void removeSd (int sd, struct pollfd* sds, int len) {
    int i;
    for (i = 0; i < len; i++) {
        if (sds[i].fd == sd) {
            sds[i].fd = -1;
            return;
        } 
    }

    puts("Can't find sd");
}

void closeClient(int sd, struct pollfd* sds, int len, list_t* conns) {
    if (shutdown(sd, SHUT_RDWR)) {
        perror("Shutdown failed");
        exit(-1);
    }
    
    if (close(sd)) {
        perror("Close failed");
        exit(-1);
    }
    printf("Closed %d\n", sd);
    
    // Remove from lists
    struct connection c;
    c.sd = sd;
    
    removeSd(sd, sds, len);
    
    struct list_elem* le = list_find(conns, (char*) &c, cmpSd);
    if (le == NULL) {
        printf("Did not find connection in list\n");
        exit(-1);
    }
    free(le->data);                                
    list_remove(conns, le);
}

void closeAll(list_t* conns, int eSd, int hdSd) {

}

int bind_socket (int sd, struct sockaddr_in eSin, int port) {
    eSin.sin_family = AF_INET;
    eSin.sin_port = htons(port);
    eSin.sin_addr.s_addr = INADDR_ANY;

    if (bind(sd, (struct sockaddr *) &eSin, sizeof(eSin)) != 0) {
        perror("Binding failed");
        close(sd);
        exit(-1);
    }
    return sd;
}

void listen_socket(int sd) {
    if (listen(sd, MAX_CONN) != 0) {
        perror("Listen failed");
        close(sd);
        exit(-1);
    }
}

int main (int argc, char** argv) {
    int echoSock;
    int hdSock;
    struct sockaddr_in  eSin, hdSin;

    int serverTimeout = SERVER_TIMEOUT;

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

    puts("[echo]: Created");

    if ((hdSock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(-1);
    }

    puts("[hexdump]: Created");

    // Fill eSin
    bind_socket(echoSock, eSin, ECHO_PORT);
    puts("[echo]: Bound");
    bind_socket(hdSock, hdSin, HD_PORT);
    puts("[hexdump]: Bound");

    listen_socket(echoSock);
    puts("[echo]: Listening...");
    listen_socket(hdSock);
    puts("[hexdump]: Listening...");

    struct pollfd sds[MAX_CONN + SERVICE_COUNT];
    // Init pollfds
    int i;
    for (i = 0; i < MAX_CONN + SERVICE_COUNT; i++) {
        sds[i].fd = -1;
        sds[i].events = POLLIN;
    }
    sds[0].fd = echoSock;
    sds[1].fd = hdSock;

    struct timeval startPoll;
    struct timeval endPoll;
    while (1) {
        gettimeofday(&startPoll, NULL);
        int pollRet = poll(sds, MAX_CONN + SERVICE_COUNT, nextTimeout(conns, serverTimeout));
        gettimeofday(&endPoll, NULL);
        int elapsed = (endPoll.tv_sec - startPoll.tv_sec) * 1000 + (endPoll.tv_usec - startPoll.tv_usec) / 1000;
        printf("Elapsed: %d\n", elapsed);
        updateTimeouts(conns, elapsed);
        serverTimeout -= elapsed;
        printf("t:%d p:%d\n", serverTimeout, pollRet);
        
        // printf("%d\n", pollRet);
        
        if (pollRet < 0) {
            perror("Poll failed");
            close(echoSock);
            exit(-1);
        } else if (pollRet == 0 && serverTimeout <= 0) {
            puts("Server timeout");
            break;
        }

        for (i = 0; i < MAX_CONN + SERVICE_COUNT; i++) {
            if (sds[i].fd < 0 || sds[i].revents == 0) {
                continue;
            }
            
            if (sds[i].revents & POLLHUP) {
                closeClient(sds[i].fd ,sds, MAX_CONN + SERVICE_COUNT, conns);
            } else if (sds[i].revents & POLLIN) {
                if (sds[i].fd == echoSock) {
                    // Accept
                    struct sockaddr_in clientAddr;
                    unsigned int clientLen = 0;
                    int clientSd;

                    // printf("SD: %d, clientAddr: %x\n", echoSock, &clientAddr);

                    if ((clientSd = accept(echoSock, (struct sockaddr *) &clientAddr, &clientLen)) < 0) {
                        perror("Accept failed");
                        close(echoSock);
                        exit(-1);
                    }

                    printf("[echo]: Accepted %d\n", clientSd);

                    addSd(clientSd, sds, MAX_CONN + SERVICE_COUNT);

                    struct connection* conn = malloc(sizeof(struct connection));
                    if (conn == NULL) {
                        perror("Can't allocate new connection");
                        exit(-1);
                    }
                    conn->sd = clientSd;
                    conn->service = echo;
                    conn->timeout = CONN_TIMEOUT;

                    serverTimeout = SERVER_TIMEOUT;

                    list_append(conns, (char*) conn);
                } else if (sds[i].fd == hdSock) {
                    struct sockaddr_in clientAddr;
                    unsigned int clientLen = 0;
                    int clientSd;

                    if ((clientSd = accept(hdSock, (struct sockaddr *) &clientAddr, &clientLen)) < 0) {
                        perror("Accept failed");
                        close(hdSock);
                        exit(-1);
                    }

                    printf("[hexdump]: Accepted %d\n", clientSd);

                    addSd(clientSd, sds, MAX_CONN + SERVICE_COUNT);

                    struct connection* conn = malloc(sizeof(struct connection));

                    if (conn == NULL) {
                        perror("Can't allocate new connection");
                        exit(-1);
                    }
                    conn->sd = clientSd;
                    conn->service = hexdump_s;
                    conn->timeout = CONN_TIMEOUT;

                    serverTimeout = SERVER_TIMEOUT;

                    list_append(conns, (char*) conn);
                } else {
                    // Echo
                    printf("[echo]: Communicating with %d\n", sds[i].fd);
                    int end = recv(sds[i].fd, buffer, sizeof(buffer) - 1, 0);
                    if (end == 0) {
                        puts("EOF");
                        closeClient(sds[i].fd, sds, MAX_CONN + SERVICE_COUNT, conns);
                        continue;
                    } else if (end < 0) {
                        puts ("end < 0");
                        exit(-1);
                    }

                    struct connection search;
                    search.sd = sds[i].fd;
                    struct list_elem* le = list_find(conns, (char*) &search, cmpSd);
                    if (le == NULL) {
                        printf("Did not find connection in list\n");
                        exit(-1);
                    }

                    struct connection* c = (struct connection*) le->data;
                    c->timeout = CONN_TIMEOUT;
                    serverTimeout = SERVER_TIMEOUT;
                    
                    switch (c->service)
                    {
                        case echo:
                            write(sds[i].fd, buffer, end);
                            break;
                        case hexdump_s:
                            hexdump(sds[i].fd, buffer, end);
                            break;
                        default:
                            perror("Unknown type\n");
                            exit(-1);
                            break;
                    }
                }
            }
        }

        // Close connections that timed out
        struct list_elem* last = NULL;
        struct list_elem* le = conns->first;
        while (le != NULL) {
            last = le;
            le = le->next;
            struct connection* c = (struct connection*) last->data;
            if (c->timeout <= 0) {
                printf("Connection %d timeout\n", c->sd);
                closeClient(c->sd, sds, MAX_CONN + SERVICE_COUNT, conns);
            }
        }
    }

    list_finit(conns);
    free(conns);
    close(echoSock);
    close(hdSock);
    return 0;
}
