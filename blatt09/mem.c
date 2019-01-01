#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "list.h"
#include "memory.h"


main(int argc, char const *argv[])
{
    if (argc < 2) {
        perror("Not enough arguments");
        return -1;
    }

    int c;
    unsigned int memory_size = 0;
    unsigned int blocksize = 0;
    unsigned int index = 0;
    unsigned int size = 0;
    enum mem_algo strategy = none;
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
                mem_init(memory_size, blocksize, strategy);
                break;
            case 'w':
                strategy = worst;
                //printf("W\n");
                mem_init(memory_size, blocksize, strategy);
                break;
            case 'a':
                size = atoi(optarg);
                //printf("A: %d\n", size);
                list_append(list, mem_alloc(size));
                break;
            case 'f':
                index = atoi(optarg);
                //printf("F: %d\n", index);
                mem_free(list_nth(list, index)->data->addr);
                list_remove(list, list_nth(list, index));
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
                return -1;
        }
    }
}
