#include <string.h>
#include <stdio.h>

int main() {

  char str[] = "boe]!ai0FB@.:|L6l@A?>qJ}I";
  int i = 0;
  int rot = 0;
  int len = strlen(str);

  while (i < len) {
    if (rot == 6)
      rot = 0;

    if (i % 2 == 0) {
      for (int j = 0; j < "0123456"[rot]; j++) {
        str[i] = str[i] - 1;
        if (str[i] == '\x1f') {
          str[i] = '~';
        }
      }
    } else {
      for (int j = 0; j < "0123456"[rot]; j++) {
        str[i] = str[i] + 1;
        if (str[i] == '\x7f') {
          str[i] = ' ';
        }
      }
    }
    rot++;
    i++;
  }

  printf("Decoded string: %s\n", str);
  return 0;
}