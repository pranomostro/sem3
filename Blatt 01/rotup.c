#include <stdio.h>
#include <unistd.h>


#define MAX_LINE_SZ 1024

int main() {
  char buf[MAX_LINE_SZ];
  read(stdin, buf, MAX_LINE_SZ);
  return 0;
}
