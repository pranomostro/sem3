#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>

#include "list.h"

struct threadInfo {
    int k;
    int r;
};

void printContext(threadcontext_t* t) {
    printf("%ld\n", t->thread);
}

void countSeconds(int k, int random) {
    if (random) {
        k = rand() % ((int) (1.5 * k) - k / 2) + k / 2;
    }

    for (int i = 1; i <= k; i++) {
        int left = sleep(1);
        // in case of interrupt
        while (left != 0) {
           left = sleep(left);
        }
        printf("%10u\t%d\t%ld\n", pthread_self(), getpid(), i);
    }
}

void* threadJob(void* arg) {
    struct threadInfo* info = (struct threadInfo*) arg;
    countSeconds(info->k, info->r);
    free(info);
    return NULL; 
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
    list_t* list = list_init();

    // Init rand
    srand(getpid());

    time_t rtime;
    struct tm *timeinfo;
    time(&rtime);
    timeinfo = localtime(&rtime);
    printf("Start: %s", asctime(timeinfo));

    for (int i = 0; i < n; i++) {
        pthread_t* thread = malloc(sizeof(pthread_t));
        struct threadInfo* info = malloc(sizeof(struct threadInfo));
        info->k = k;
        info->r = r;
        if (pthread_create(thread, NULL, threadJob, info)) {
            fprintf(stderr, "Error creating thread.\n");
            exit(-1);
        }
        threadcontext_t* context = malloc(sizeof(threadcontext_t));
        context->thread = *thread;
        list_append(list, context);
    }

    // list_print(list, printT);

    for (int i = 0; i < n; i++) {
        int s;
        if (s = pthread_join(list->first->data->thread, NULL)) {
            switch (s) {
                case EINVAL:
                    printf("inval\n");
                    break;
                case ESRCH:
                    printf("not found\n");
                    break;
            }
            fprintf(stderr, "Join failed.\n");
            exit(-1);
        }
        free(list->first->data);
        list_remove(list, list->first);
    }
    
    time(&rtime);
    timeinfo = localtime(&rtime);
    printf("Ende: %s", asctime(timeinfo));

    // Deallocate list
    list_finit(list);

    return 0;
}
