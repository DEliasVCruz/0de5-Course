#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PRINT_USAGE                                                            \
  printf("Usage: %s [-p style] [-m alloc_mode] number\n", argv[0])
#define INVALID_NUMBER_ERROR_MESSAGE                                           \
  printf("Pass a number between 1 and %ld\n", LONG_MAX)

typedef enum { VERTICAL, HORIZONTAL } orientation_t;
typedef enum { BUFFER, MALLOC } allocation_t;

int FizzSize = sizeof("Fizz");
int BuzzSize = sizeof("Buzz");
int FizzBuzzSize = sizeof("Fizzbuzz");

void printFizzBuzzVertical(long);
char *getFizzBuzzStringMalloc(long);

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Usage: %s number\n", argv[0]);

    return 0;
  }

  orientation_t orientation = VERTICAL;
  allocation_t alloc_mode = MALLOC;

  int opt;
  while ((opt = getopt(argc, argv, "p:m:")) != -1) {
    switch (opt) {
    case 'p':
      if (strncmp("v", optarg, 2) == 0) {
        orientation = VERTICAL;
      } else if (strncmp("h", optarg, 2) == 0) {
        orientation = HORIZONTAL;
      } else {
        printf("Unrecognized print style %s\n", optarg);
        PRINT_USAGE;

        return 1;
      }

      break;
    case 'm':
      if (strncmp("b", optarg, 2) == 0) {
        alloc_mode = BUFFER;
      } else if (strncmp("h", optarg, 2) == 0) {
        alloc_mode = MALLOC;
      } else {
        printf("Unrecognized allocation mode %s\n", optarg);
        PRINT_USAGE;

        return 1;
      }

      break;
    default:
      PRINT_USAGE;
      return 1;
    }
  }

  if (optind >= argc) {
    fprintf(stderr, "Expected argument after options\n");
    PRINT_USAGE;
    return 1;
  }

  char *value_str = argv[optind];
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
    INVALID_NUMBER_ERROR_MESSAGE;
    PRINT_USAGE;

    return 1;
  }

  if (*endptr != '\0') {
    INVALID_NUMBER_ERROR_MESSAGE;
    PRINT_USAGE;

    return 1;
  }

  if (value <= 0) {
    INVALID_NUMBER_ERROR_MESSAGE;
    PRINT_USAGE;

    return 1;
  }

  if (orientation == VERTICAL) {
    printFizzBuzzVertical(value);

    return 0;
  }

  char *fizz_buzz_string;

  if (alloc_mode == MALLOC) {
    fizz_buzz_string = getFizzBuzzStringMalloc(value);
  }

  printf("%s\n", fizz_buzz_string);

  return 0;
}

void printFizzBuzzVertical(long value) {
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
}

char *getFizzBuzzStringMalloc(long value) {
  int buffer_capacity = ceil(value / 2.) * 8 + (value) * 2 + 2;
  int buffer_length = 0;

  char *buffer = (char *)malloc(buffer_capacity);
  int max_write = buffer_capacity - buffer_length;

  int size;

  for (long i = 1; i <= value; i++) {
    if ((i % 3 == 0) && (i % 5 == 0)) {
      size = snprintf(&buffer[buffer_length], max_write, "%s", "Fizzbuzz, ");
      assert(size > 0 && size < max_write);

      buffer_length += size;
      max_write = buffer_capacity - buffer_length;

      continue;
    }

    if (i % 3 == 0) {
      size = snprintf(&buffer[buffer_length], max_write, "%s", "Fizz, ");
      assert(size > 0 && size < max_write);

      buffer_length += size;
      max_write = buffer_capacity - buffer_length;

      continue;
    }

    if (i % 5 == 0) {
      size = snprintf(&buffer[buffer_length], max_write, "%s", "Buzz, ");
      assert(size > 0 && size < max_write);

      buffer_length += size;
      max_write = buffer_capacity - buffer_length;

      continue;
    }

    size = snprintf(&buffer[buffer_length], max_write, "%ld, ", i);
    assert(size > 0 && size < max_write);

    buffer_length += size;
    max_write = buffer_capacity - buffer_length;
  }

  snprintf(&buffer[buffer_length - 2], max_write, "%s", ".");
  assert(size > 0 && size < max_write);

  return buffer;
}
