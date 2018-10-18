#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>


#define MAX_LINE_SZ 1024

// Uses functions from the "C-Vorkurs"
char* rotup(char* toEncode, int rotations) {
  char* encoded = strdup(toEncode);

  int i = 0;
  while (toEncode[i] != '\0') {
    if (isalpha(toEncode[i])) {
      encoded[i] = toupper(toEncode[i]);
      encoded[i] = (encoded[i] - 'A' + rotations) % 26 + 'A';
      i++;
    } else {
      // No letter --> abort conversion
      encoded[i] = '\0';
      return encoded;
    }
  }
  return encoded; 
}

int main() {
  char buf[MAX_LINE_SZ];
  int readChars = read(0, buf, sizeof(buf)); // 0 is stdin
  buf[readChars] = '\0';  // Append a NUL char since read doesn't do this
  printf("%s", buf);
  return 0;
}
