#include <stdio.h>
#include <unistd.h>


#define MAX_LINE_SZ 1024

int main() {
  char buf[MAX_LINE_SZ];
  int readChars = read(0, buf, sizeof(buf)); // 0 is stdin
  buf[readChars] = '\0';  // Append a NUL char since read does'nt do this
  printf("%s", buf);
  return 0;
}
