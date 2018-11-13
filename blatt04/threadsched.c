#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "list.h"

#define RR 0
#define PRR 1
#define SRTN 2

void print_time_step (int time, int thread_num);

void p(threadcontext_t *c) {
    printf("N: %d, Prio: %d, Start: %d, Target: %d\n", c->n, c->prio, c->start, c->target);
}

int main(int argc, char** argv) {
    int n, t, q, a;
    // Parse input
    int c;
    char* nVal = NULL;
    char* tVal = NULL;
    char* qVal = NULL;
    char* aVal = NULL;
    while ((c = getopt(argc, argv, "n:t:q:a:")) > -1) {
        switch (c) {
            case 'n':
                nVal = optarg;
                break;
            case 't':
                tVal = optarg;
                break;
            case 'q':
                qVal = optarg;
                break;
            case 'a':
                aVal = optarg;
                break;
            case '?':
                if (optopt == 'n' || optopt == 't' || optopt == 'q' || optopt == 'a') {
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
    if (nVal != NULL) {
        for(char* p = nVal; *p != '\0'; p++) {
            if (!isdigit(*p)) {
                puts("N must be an integer");
                exit(-1);
            }
        }

        n = atoi(nVal);
    }

    if (tVal != NULL) {
        for(char* p = nVal; *p != '\0'; p++) {
            if (!isdigit(*p)) {
                puts("T must be an integer");
                exit(-1);
            }
        }

        t = atoi(tVal);
    }

    if (qVal != NULL) {
        for(char* p = nVal; *p != '\0'; p++) {
            if (!isdigit(*p)) {
                puts("Q must be an integer");
                exit(-1);
            }
        }

        q = atoi(qVal);
    }

    if (aVal != NULL) {
        if (!strcmp(aVal, "RR")) {
            a = RR;
        } else if (!strcmp(aVal, "PRR")) {
            a = PRR;
        } else if (!strcmp(aVal, "SRTN")) {
            a = SRTN;
        } else {
            fprintf(stderr, "Unknown algorithm %s\n", aVal);
            exit(-1);
        }
    }

    printf("%d %d %d %d\n", n, t, q, a);

    char buf[1024];
    int params[3];
    int pIndex;
    list_t *list = list_init();

    for(int i = 0; i < n; i++) {
        fgets(buf, 1023, stdin);
        char *l = buf;
        pIndex = 0;
        for(char* p = buf; *p != '\0'; p++) {
            if (*p == ' ' || *p == '\n') {
                *p = '\0';
                for(char *q = l; q != p; q++) {
                    if (!isdigit(*q)) {
                        fprintf(stderr, "Expected integer\n");
                        exit(-1);
                    }
                }
                if (pIndex >= 3) {
                    fprintf(stderr, "Expected 3 Parameters\n");
                    exit(-1);
                }
                params[pIndex++] = atoi(l);
                l = p+1;
            }
        }
        threadcontext_t *context = malloc(sizeof(threadcontext_t));
        context->n = i + 1;
        context->prio = params[0];
        context->start = params[1];
        context->target = params[2];
        list_append(list, context);
    }
    list_print(list, p);

}

void print_time_step (int time, int thread_num) {
    static int    first_time = 1;
    int           i;

    if (first_time) {
    printf ("  Time |  1  2  3  4  5  6  7  8  9  10\n");
    printf ("-------+--------------------------------\n");
    first_time = 0;
    }
    printf ("%06d |", time);
    if (thread_num) {
    for (i = 1; i < thread_num; i++)
        printf ("   ");
    printf ("  %d\n", thread_num);
    } else
    printf ("\n");
}

#undef RR
#undef PRR
#undef SRTN