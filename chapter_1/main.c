#include <stdio.h>

int main() {
  char input = '\0';

  while (input != 'Q') {
    printf("Enter a single character\n");
    printf("> ");

    input = getc(stdin);
    if (input == '\n') {
      continue;
    }

    for (int c; (c = getc(stdin)) != '\n' && c != EOF;) {
      input = '\0';
    }
  }

  printf("Welcome to 0DE5!\n");

  return 0;
}
