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
  ssize_t readChars = read(0, buf, sizeof(buf)); // 0 is stdin
  if (readChars < 0) {
    write(2, "Error during read.", 18);
  }
  // Append a NUL char since read doesn't do this
  if (buf[readChars - 1] == '\n') {
    buf[readChars - 1] = '\0';
  } else {
    buf[readChars] = '\0';
  }
  write(1, "Hallo: ", 7);
  write(1, buf, readChars);
  write(1, " -- ", 4);
  char* code = rotup(buf, 13);
  write(1, code, strlen(code));
  write(1, "\n", 1);
  return 0;
}
