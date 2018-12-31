#include <stdio.h>

#include "list.h"
#include "memory.h"


main(int argc, char const *argv[])
{
    list_t *list = list_init();
    mem_init(10000, 1000, first);
    list_insert(list, mem_alloc(1000));
    mem_free(list->first->data->addr);
    printf("Beende Programm\n");
    return 0;
}
