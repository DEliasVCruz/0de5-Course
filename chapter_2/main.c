#include <stdio.h>

void print_binary(char byte);

int main() {
  char string[15] = {0b01001101, 0x79,       0b00100000, 0b01101110, 0x61,
                     0x6D,       0b01100101, 0x20,       0x69,       0x73,
                     0b00101110, '.',        0b00101110, 0b00100000, '\0'};

  printf("As hex:    ");
  for (int i = 0; i < 15; i++) {
    char byte = string[i];

    if (byte == '\0') {
      break;
    }

    printf(" %8x", byte);
  }
  printf("\n");

  printf("As decimal:");
  for (int i = 0; i < 15; i++) {
    char byte = string[i];

    if (byte == '\0') {
      break;
    }

    printf(" %8d", byte);
  }
  printf("\n");

  printf("As binary: ");
  for (int i = 0; i < 15; i++) {
    char byte = string[i];

    if (byte == '\0') {
      break;
    }

    print_binary(byte);
  }
  printf("\n");

  printf("%s Daniel Vilela\n", string);

  return 0;
}

void print_binary(char byte) {
  printf(" ");

  for (int i = 7; i >= 0; i--) {
    char bit = byte & 1 << i;

    printf("%d", bit >> i);
  }
}
