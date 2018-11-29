#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "list.h"

#define MAX_SZ 1024

typedef struct proc {
    int pid;
    // More to be added...
} process;

extern bool parse(char* str, list_t* paramList, char** envp);

int cmpPid (const char* p1, const char* p2) {
    return ((process*) p1)->pid - ((process*) p2)->pid;
}

int main (int argc, char** argv, char** envp) {
    list_t* processList = list_init();
    if (processList == NULL) {
        perror("Cannot allocate list\n");
        exit(-1);
    }

    char* path = getenv("PATH");

    while (true) {
        printf("$ ");
        fflush(stdout);
        char* buf = malloc(MAX_SZ * sizeof(char));
        if (buf == NULL) {
            perror("Cannot allocate buffer\n");
            exit(-1);
        }
        fgets(buf, MAX_SZ, stdin);
        for (char* p = buf; *p != '\0'; p++) {
            if (*p == '\n') {
                *p = '\0';
                break;
            }
        }

        list_t* paramList = list_init();
        if (paramList == NULL) {
            perror("Cannot allocate list\n");
            exit(-1);
        }

        bool stop = !parse(buf, paramList, envp);

        free(buf);

        if (paramList->first != NULL && !stop) {
            char** args = list_to_array(paramList);
            /*char* args[3];
            args[0] = paramList->first->data;
            args[1] = "Hello";
            args[2] = NULL;*/
            pid_t pid = fork();
            switch (pid) {
                case -1:
                    perror("Fork failed");
                    exit(-1);
                case 0:
                    // Child
                    if (strchr(args[0], '/') != NULL) {
                        execve(args[0], args, envp);
                    } else {
                        int comLen = strlen(args[0]);
                        char* tokPath = strdup(path);
                        char* tok = strtok(tokPath, ":");
                        while (tok != NULL) {
                            // printf("%d\n", strlen(tok) + comLen + 2);
                            char* withCom = strdup(tok);
                            char* tmp = realloc(withCom, strlen(withCom) + comLen + 2);
                            if (tmp != NULL) {
                                withCom = tmp;
                            } else {
                                exit(-1);
                            }
                            withCom = strcat(strcat(withCom, "/"), args[0]);
                            // puts(withCom);
                            execve(withCom, args, envp);
                            // puts("Not this");
                            free(withCom);
                            tok = strtok(NULL, ":");
                        }
                    }
                    perror("Command not found");
                    exit(-1);
                default: {
                    // Parent
                    process* child = malloc(sizeof(process));
                    child->pid = pid;
                    list_append(processList, (char*) child);
                    break;
                }
            }
        }

        if (!stop) {
            pid_t retPid = wait(NULL);
            // puts("Join");
            process retProc = {retPid};
            struct list_elem* inList = list_find(processList, (char*) &retProc, cmpPid);
            free(inList->data);
            list_remove(processList, inList);
        }

        // Free paramList
        list_finit(paramList);
        free(paramList);

        if (stop) {
            break;
        }
    }

    return 0;
}