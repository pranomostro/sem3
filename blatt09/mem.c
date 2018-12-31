#include <stdio.h>

#include "memory.h"


main(int argc, char const *argv[])
{
    mem_init(10000, 1000, first);
    mem_alloc(1000);
    printf("Beende Programm\n");
    return 0;
}
