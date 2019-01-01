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
    int boolean = 0;
    list_t *list = list_init();

    while ((c = getopt(argc, argv, "m:b:a:f:1w")) > -1) {
        switch (c) {
            case 'm':
                memory_size = atoi(optarg);
                //printf("M: %d\n", memory_size);
                if (boolean == 1 && blocksize > 0 && strategy != none) {
                    boolean = 0;
                    mem_init(memory_size, blocksize, strategy);
                }
                break;
            case 'b':
                blocksize = atoi(optarg);
                //printf("B: %d\n", blocksize);
                if (boolean == 1 && memory_size > 0 && strategy != none) {
                    boolean = 0;
                    mem_init(memory_size, blocksize, strategy);
                }
                break;
            case '1':
                strategy = first;
                //printf("1\n");
                if (memory_size > 0 && blocksize > 0) {
                    mem_init(memory_size, blocksize, strategy);
                } else  {
                    boolean = 1;
                }
                break;
            case 'w':
                strategy = worst;
                //printf("W\n");
                if (memory_size > 0 && blocksize > 0) {
                    mem_init(memory_size, blocksize, strategy);
                } else  {
                    boolean = 1;
                }
                break;
            case 'a':
                size = atoi(optarg);
                //printf("A: %d\n", size);
                list_append(list, mem_alloc(size));
                break;
            case 'f':
                index = atoi(optarg);
                struct list_elem *elem = NULL;
                if ((elem = list_nth(list, index)) == NULL) {
                    perror("List_nth doesn't find the elem expected");
                    continue;
                }
                //printf("F: %d got %p\n", index, elem->data->addr);
                mem_free(&elem);
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
