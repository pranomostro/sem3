#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

int countSeconds(int k) {
    int pid = getpid();
    int ppid = getppid();

    for (int i = 1; i <= k; i++) {
        int left = sleep(1);
        // in case of interrupt
        while (left != 0) {
           left = sleep(left);
        }
        printf("%d %d %d\n", pid, ppid, i);
    }

    return (pid + k) % 100;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        puts("Usage: forkone <K>");
        exit(-1);
    }

    time_t rtime;
    struct tm *timeinfo;
    time(&rtime);
    timeinfo = localtime(&rtime);
    printf("Start: %s", asctime(timeinfo));

    for(char* p = argv[1]; *p != '\0'; p++) {
        if (!isdigit(*p)) {
            puts("K must be an integer");
            exit(-1);
        }
    }

    int k = atoi(argv[1]);

    pid_t forkRet = fork();

    int status;
    switch (forkRet) {
        case 0:
            // Child
            exit(countSeconds(k));
        default:
            // Parent
            wait(&status);
            // Child terminated
            // get exit status
            if (WIFEXITED(status)) {
                status = WEXITSTATUS(status);
            }
            
            printf("Exit-Code: %d\n", status);
            time(&rtime);
            timeinfo = localtime(&rtime);
            printf("Ende: %s", asctime(timeinfo));
            break;
    }

    return 0;
}