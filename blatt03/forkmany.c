#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <math.h>

#include "list.h"

int countSeconds(int k, int random) {
    int pid = getpid();
    int ppid = getppid();

    if (random) {
        k = rand() % ((int) (1.5 * k) - k / 2) + k / 2;
    }

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
    int c;
    char* kVal = NULL;
    char* nVal = NULL;
    while ((c = getopt(argc, argv, "k:n:r")) > -1) {
        switch (c) {
            case 'k':
                kVal = optarg;
                break;
            case 'n':
                nVal = optarg;
                break;
            case 'r':
                r = 1;
                break;
            case '?':
                if (optopt == 'k' || optopt == 'n') {
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                    exit(-1);
                } else {
                    fprintf(stderr, "Unknown option -%c", optopt);
                    exit(-1);
                }
                break;
            default:
                abort();
        }
    }

    // Check input
    if (kVal != NULL) {
        for(char* p = kVal; *p != '\0'; p++) {
            if (!isdigit(*p)) {
                puts("K must be an integer");
                exit(-1);
            }
        }

        k = atoi(kVal);
    }

    if (nVal != NULL) {
        for(char* p = nVal; *p != '\0'; p++) {
            if (!isdigit(*p)) {
                puts("N must be an integer");
                exit(-1);
            }
        }

        n = atoi(nVal);
    }

    // Init list
    // list_t* list = list_init();

    time_t rtime;
    struct tm *timeinfo;
    time(&rtime);
    timeinfo = localtime(&rtime);
    printf("Start: %s", asctime(timeinfo));

    for (int i = 0; i < n; i++) {
        pid_t forkRet = fork();
        if (forkRet == 0) {
            // Child
            
            // Init rand
            srand(getpid());
            
            exit(countSeconds(k, r));
        } else if (forkRet == -1) {
            // Error
            perror("Fork failed");
            exit(-1); 
        }
        
        // char* buf = malloc(1024 * sizeof(char));
        // sprintf(buf, "%d", forkRet);
        // printf("%d buf: %s\n", forkRet, buf);
        // list_append(list, buf);
    }

    // list_print(list, puts);

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