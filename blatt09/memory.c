#include <stdlib.h>
#include <stdio.h>

#include "memory.h"
#include "list.h"

unsigned int BLOCKSIZE;
unsigned int SIZE;
enum mem_algo STRATEGY;
list_t *list;

int mem_init(unsigned int size, unsigned int blocksize, enum mem_algo strategy) {
    /*Init the control vars */
    SIZE = size;
    BLOCKSIZE = blocksize;
    STRATEGY = strategy;
    list = list_init();
    mem_dump();
    return list;
}

void *add_memblock(unsigned int size, struct list_elem *current) {
    struct memblock *elem = malloc(sizeof(struct memblock));
    struct memblock *remaining = malloc(sizeof(struct memblock));
    elem->status = used;
    remaining->status = free_;
    
    int blocks = size / BLOCKSIZE;
    elem->size = blocks * BLOCKSIZE;
    remaining->size = SIZE - elem->size;
    
    elem->in_use = size;
    remaining->in_use = 0;

    elem->addr = malloc(sizeof(struct memblock));
    remaining->addr = malloc(sizeof(struct memblock));
    list_remove(list, list->last);
    list_append(list, elem);
    list_append(list, remaining);
    mem_dump();
    return elem;
}

void *mem_alloc (unsigned int size) {
    
    struct list_elem *temp = list->first;
    if (temp == NULL) {
        add_memblock(size, NULL);
    } else {
        int blocks = (size / BLOCKSIZE) * BLOCKSIZE;
        int worst_value = 0;
        struct list_elem *worst_elem = NULL;
        switch (STRATEGY)
        {
            case none:
            case first:
                for( ; temp != NULL; temp = temp->next) {
                    if (temp->data->status == free_ && temp->data->size >= blocks && temp->data->in_use <= blocks) {
                        return add_memblock(size, temp);
                    }
                }
                break;
            case worst:
                for( ; temp != NULL; temp = temp->next) {
                    if (temp->data->status == free_ && temp->data->size >= blocks && temp->data->size - temp->data->in_use > worst_value) {
                        worst_value = temp->data->size - temp->data->in_use;
                        worst_elem = temp;
                    }
                }
                if (worst_elem != NULL) {
                    worst_elem->data->status = used;
                    worst_elem->data->in_use = blocks;
                    worst_elem->data->size = blocks;
                    mem_dump();
                    return worst_elem;
                }
                break;
            default:
                //Fehlerbehandlung
                break;
        }
    }
    mem_dump();
}

int cmp (const char *a, const char *b) {
    if (a == b) {
        return 0;
    }
    return -1;
}

void mem_free (void *addr) {
    struct list_elem *elem = list_find(list, addr, cmp);
    if (elem != NULL) {
        elem->data->in_use = 0;
        elem->data->status = free_;
    }
    mem_dump();
}

void print_elem(struct list_elem *elem) {
    if (elem->data->status == free_) {
        printf("[%p %c %d/%d] ", elem->data->addr, 'F', elem->data->in_use, elem->data->size);
    } else {
        printf("[%p %c %d/%d] ", &elem->data, 'P', elem->data->in_use, elem->data->size);
    }
}

void mem_dump () {
    list_print(list, print_elem);
    printf("\n");
}