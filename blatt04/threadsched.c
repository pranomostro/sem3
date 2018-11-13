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