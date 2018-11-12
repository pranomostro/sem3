#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "list.h"

#define RR 0
#define PRR 1
#define SRTN 2

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

#undef RR
#undef PRR
#undef SRTN