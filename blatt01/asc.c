#include <stdio.h>

int main() {
  printf("Oct\tDec\tHex\tChar\n");
  char c = 0;
  for (int i = 0; i < 128;i++) {
    printf("%03o\t%-3d\t%02x\t%C\n", c, c, c, c);
    c++;
  }
  return 0;
}
