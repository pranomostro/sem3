#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "list.h"

#define MAX_SZ 1024

extern bool parse(char* str, list_t* paramList, char** envp);

void p(char* s) {
    puts(s);
}

int main (int argc, char** argv, char** envp) {
    while (true) {
        printf("$ ");
        char* buf = malloc(MAX_SZ * sizeof(char));
        if (buf == NULL) {
            perror("Cannot allocate buffer");
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
            perror("Cannot allocate list");
            exit(-1);
        }

        bool stop = !parse(buf, paramList, envp);

        free(buf);

        list_print(paramList, p);

        // Free paramList
        while (paramList->first != NULL) {
            free(paramList->first->data);
            list_remove(paramList, paramList->first);
        }

        list_finit(paramList);

        free(paramList);

        if (stop) {
            break;
        }
    }

    return 0;
}