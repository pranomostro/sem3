#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_SZ 1024

int main(int argc, char** argv) {
    int status = 1;
    char buf[MAX_LINE_SZ];
    while (status) {
        char* fgetsRet = fgets(buf, MAX_LINE_SZ, stdin);
        if (fgetsRet == NULL || strncmp("exit\n", buf, 5) == 0) {
            status = 0;
        } else {
            system(buf);
        }
    }
    return 0;
}