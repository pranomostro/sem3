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
    // Default values
    int k = 10;
    int n = 1;
    int r = 0;

    // Parse input
    // Check input

    time_t rtime;
    struct tm *timeinfo;
    time(&rtime);
    timeinfo = localtime(&rtime);
    printf("Start: %s", asctime(timeinfo));

    for (int i = 0; i < n; i++) {
        pid_t forkRet = fork();
        if (forkRet == 0) {
            // Child
            exit(countSeconds(k));
        } else if (forkRet == -1) {
            // Error
            perror("Fork failed");
            exit(-1); 
        }
    }

    // Parent
    int status;
    for (int i = 0; i < n; i++) {
        waitpid(-1, &status, 0); // equivalent to wait(&status), but wanted to try this one ^^

        // Child terminated
        // get exit status
        if (WIFEXITED(status)) {
            status = WEXITSTATUS(status);
        }

        printf("Exit-Code: %d\n", status);
    }
    
    time(&rtime);
    timeinfo = localtime(&rtime);
    printf("Ende: %s", asctime(timeinfo));

    return 0;
}