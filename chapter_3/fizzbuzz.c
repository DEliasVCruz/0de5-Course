#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Usage: %s number\n", argv[0]);

    return 0;
  }

  char *value_str = argv[1];
  char *endptr;

  errno = 0; // To distinguish success failure after call
  long value = strtol(value_str, &endptr, 10);

  // Check for out of bounds errors in asigment
  if ((errno == ERANGE && (value == LONG_MIN || value == LONG_MAX)) ||
      (errno != 0 && value == 0)) {
    perror("strtol");

    return 1;
  }

  if (endptr == value_str) {
    printf("Pass a valid number to begin\n");
    printf("Usage: %s number\n", argv[0]);

    return 1;
  }

  if (*endptr != '\0') {
    printf("Pass a valid number to begin\n");
    printf("Usage: %s number\n", argv[0]);

    return 1;
  }

  if (value == 0) {
    printf("Pass a number greater than 0\n");
    printf("Usage: %s number\n", argv[0]);

    return 1;
  }

  for (long i = 1; i <= value; i++) {
    if ((i % 3 == 0) && (i % 5 == 0)) {
      printf("Fizzbuzz\n");

      continue;
    }

    if (i % 3 == 0) {
      printf("Fizz\n");

      continue;
    }

    if (i % 5 == 0) {
      printf("Buzz\n");

      continue;
    }

    printf("%ld\n", i);
  }

  return 0;
}
