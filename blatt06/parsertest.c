#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_SZ 1024

extern bool parse(char* str, char** envp);

int main (int argc, char** argv, char** envp) {
    while (true) {
        printf("$ ");
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

        if (!parse(buf, envp)) {
            break;
        }

        free(buf);
    }

    return 0;
}