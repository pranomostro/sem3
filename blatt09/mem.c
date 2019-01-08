#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "list.h"
#include "memory.h"

enum status {
    nothing,
    block,
    memory,
    algo
};


main(int argc, char const *argv[])
{
    if (argc < 2) {
        perror("Not enough arguments");
        return -1;
    }

    int c;
    unsigned int memory_size = 1048576;
    unsigned int blocksize = 4096;
    unsigned int index = 0;
    unsigned int size = 0;
    enum mem_algo strategy = none;
    int init = 0;
    list_t *list = list_init();

    while ((c = getopt(argc, argv, "m:b:a:f:1w")) > -1) {
        switch (c) {
            case 'm':
                memory_size = atoi(optarg);
                //printf("M: %d\n", memory_size);
                break;
            case 'b':
                blocksize = atoi(optarg);
                //printf("B: %d\n", blocksize);
                break;
            case '1':
                strategy = first;
                //printf("1\n");
                break;
            case 'w':
                strategy = worst;
                //printf("W\n");
                break;
            case 'a':
                size = atoi(optarg);
                if (init == 0) {
                    mem_init(memory_size, blocksize, strategy);
                    init = 1;
                }
                //printf("A: %d\n", size);
                mem_alloc(size);
                break;
            case 'f':
                index = atoi(optarg);
                //printf("F: %d\n", index);
                mem_remove(index);
                break;
            case '?':
                if (optopt == 'm' || optopt == 'b' || optopt == 'a' || optopt == 'f') {
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                    exit(-1);
                } else {
                    fprintf(stderr, "Unknown option -%c", optopt);
                    exit(-1);
                }
                break;
            default:
                list_finit(list);
                return -1;
        }
    }
    list_finit(list);
    return 0;
}
