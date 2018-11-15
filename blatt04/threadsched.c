#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "list.h"

#define RR 0
#define PRR 1
#define SRTN 2

int lastQ = 0;

void print_time_step (int time, int thread_num);

int cmpTarget(const threadcontext_t *a, const threadcontext_t *b) {
    if (a->target < b->target) {
        return 1;
    }
    return -1;
}

int cmpPrio(const threadcontext_t *a, const threadcontext_t *b) {
    if (a->prio < b->prio) {
        return 1;
    }
    return -1;
}

int cmp(const threadcontext_t *a, const threadcontext_t *b) {
    if (a->n == b->n) {
        return 0;
    }
    return 1;
}

/*
* Round Robin:
* Split in equal timeslots and arrange tasks accordingly
*/

threadcontext_t *rr(list_t *ready, int t, int q, threadcontext_t *active) {
    if (lastQ + q == t || active == NULL || (active != NULL && active->target <= 0)) {
        if (active != NULL && active->target > 0) {
            list_append(ready, active);
        }
        if (ready == NULL || ready->first == NULL) 
            return NULL;
        struct list_elem *activeElem = ready->first;
        active = activeElem->data;
        list_remove(ready, activeElem);
        lastQ = t;
    }
    return active;
}
/*
* Priorty Round Robin:
* Insert prioritised tasks into the queue to "mock" a priority
*/

threadcontext_t *prr(list_t *ready, int t, int q, threadcontext_t *active) {
    if (lastQ + q == t || active == NULL || (active != NULL && active->target <= 0)) {
        // if (active != NULL && active->target > 0) {
        //     list_append(ready, active);
        // }
        // if (ready == NULL || ready->first == NULL) 
        //     return NULL;
        // int prio = list_getMax(ready, cmpPrio);
        // struct list_elem *tmp = ready->first;
        // for(int i = 0; i < ready->size; i++) {
        //     if (tmp->data->prio <= prio) {
        //         break;
        //     }
        //     if(tmp->next != NULL)
        //         tmp = tmp->next;
        //     else break;
        // }
        // active = tmp->data;
        // list_remove(ready, tmp);
        // lastQ = t;
        if (active != NULL && active->target > 0) {
            list_append(ready, active);
        }
        if (ready == NULL || ready->first == NULL) 
            return NULL;
        struct list_elem *activeElem = list_getMax(ready, cmpPrio);
        active = activeElem->data;
        list_remove(ready, activeElem);
        lastQ = t;
    }
    return active;
}

/*
* Shortest Remaining Time Next:
* Immediatly switch to the thread which has the lowest remaining computation time
*/

struct list_elem *getShortestRemaining(list_t *list, int t) {
    int shortestTime = 30001;
    if (list->first == NULL)
        return NULL;
    struct list_elem *next = list->first;
    struct list_elem *shortest = list->first;
    for(int i = 0; i < list->size; i++) {
        if(next->data->start <= t && next->data->target < shortestTime) {
            shortest = next;
            shortestTime = next->data->target;
        }
        if (next->next != NULL)
            next = next->next;
        else break;
    }
    return shortest;
}

threadcontext_t *srtn(list_t *ready, int t, int q, threadcontext_t *active) {
    // if (lastQ + q == t || active == NULL || (active != NULL && active->target <= 0)) {
    //     if (ready->first == NULL) {
    //         return NULL;
    //     }
    //     if (active != NULL && active->target > 0) {
    //         list_append(ready, active);
    //     }
    //     struct list_elem *tmp = getShortestRemaining(ready, t);
    //     active = tmp->data;
    //     list_remove(ready, tmp);
    //     lastQ = t;
    // }
    // return active;
    if (active != NULL && active->target > 0) {
        list_append(ready, active);
    }
    if (ready == NULL || ready->first == NULL) 
        return NULL;
    struct list_elem *activeElem = list_getMax(ready, cmpTarget);
    active = activeElem->data;
    list_remove(ready, activeElem);
    lastQ = t;
    return active;
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
        if (n > 10) {
            fprintf(stderr, "N > 10\n");
            exit(-1);
        }
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
        if (params[0] < 1 || params[0] > 10 || params[1] > 100000 || params[2] > 30000) {
            fprintf(stderr, "Invalid Parameters\n");
            exit(-1);
        }
        threadcontext_t *context = malloc(sizeof(threadcontext_t));
        context->n = i + 1;
        context->prio = params[0];
        context->start = params[1];
        context->target = params[2];
        list_append(list, context);
    }

    // Starting Simulation

    int millis = 0;
    threadcontext_t *active = NULL;
    list_t *ready = list_init();

    while(list->first != NULL || ready->first != NULL || active->target > 0) {
        struct list_elem *l = NULL;
        struct list_elem *next = list->first;
        // printf("List Size: %d\n", list->size);
        
        while(next != NULL) {
            l = next;
            next = l->next;
            if (l->data->start <= millis) {
                list_append(ready, l->data);
                list_remove(list, l);
                // printf("List Size after Iteration: %d\n", list->size);
            }
        }
        // printf("Leaving Loop\n");
        switch (a) {
            case RR:
                active = rr(ready, millis, q, active);
                break;
            case PRR:
                active = prr(ready, millis, q, active);
                break;
            case SRTN:
                active = srtn(ready, millis, q, active);
                break;
        }

        if (active != NULL) {
            active->target -= t;
        }

        if (list->first != NULL || ready->first != NULL || active != NULL) {
            if (active != NULL) {
                print_time_step(millis, active->n);
            } else {
                print_time_step(millis, 0);
            }
        }
        millis += t;
    }
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