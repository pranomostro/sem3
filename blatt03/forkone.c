#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <ctype.h>

int main(int argc, char** argv) {
    if (argc != 2) {
        puts("Usage: forkone <K>");
        exit(-1);
    }

    for(char* p = argv[1]; *p != '\0'; p++) {
        if (!isdigit(*p)) {
            puts("K must be an integer");
            exit(-1);
        }
    }

    int k = atoi(argv[1]);
    printf("%d\n", k);

    pid_t forkRet = fork();

    switch (forkRet) {
        case 0:
            // Child
            break;
        default:
            // Parent
            wait(NULL);
            // Child terminated
    }

    return 0;
}