#include <alloca.h>
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
#define REALLOC_IF_NECESARY(check, increase, capacity, available, length,      \
                            data, temp)                                        \
  if (check) {                                                                 \
    increase;                                                                  \
                                                                               \
    temp = (char *)malloc(capacity);                                           \
    if (temp == NULL) {                                                        \
      fprintf(stderr, "Not enough memory in system\n");                        \
      exit(1);                                                                 \
    }                                                                          \
                                                                               \
    strncpy(temp, data, capacity);                                             \
    free(data);                                                                \
                                                                               \
    data = temp;                                                               \
    temp = NULL;                                                               \
                                                                               \
    available = capacity - length;                                             \
  }
#define MINMAX(value, min, max) value <= min ? min : value >= max ? max : value

typedef enum { VERTICAL, HORIZONTAL } orientation_t;
typedef enum { BUFFER, MALLOC } allocation_t;

typedef struct {
  char *data;
  int capacity;
  int length;
  int free;
} buffer_t;

int FizzSize = sizeof("Fizz");
int BuzzSize = sizeof("Buzz");
int FizzBuzzSize = sizeof("Fizzbuzz");

void printFizzBuzzVertical(long);
char *getFizzBuzzStringMalloc(long);
long getFizzBuzzString(char *, int, long, long);

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
      } else if (strncmp("a", optarg, 2) == 0) {
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
  int buffer_size = MINMAX(value, 100, 10000);

  switch (alloc_mode) {
  case MALLOC:
    fizz_buzz_string = getFizzBuzzStringMalloc(value);

    break;
  case BUFFER:
    fizz_buzz_string = alloca(buffer_size);

    long start_idx = 1;
    while (start_idx = getFizzBuzzString(fizz_buzz_string, buffer_size,
                                         start_idx, value),
           start_idx != -1) {
      printf("%s", fizz_buzz_string);

      // We flush the string buffer
      *fizz_buzz_string = '\0';
    }

    break;
  default:
    fprintf(stderr, "unsuported allocation mode");
    return 1;
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
  int base_capacity = ceil(value / 2.) * 8 + (value) * 2 + 2;

  buffer_t str_buffer = {.data = NULL,
                         .capacity = base_capacity,
                         .length = 0,
                         .free = base_capacity};

  str_buffer.data = (char *)malloc(str_buffer.capacity);
  if (str_buffer.data == NULL) {
    fprintf(stderr, "Not enough memory in system\n");
    exit(1);
  }

  char *temp_buffer = NULL;
  int size;

  for (long i = 1; i <= value; i++) {
    if ((i % 3 == 0) && (i % 5 == 0)) {
      REALLOC_IF_NECESARY(str_buffer.free <= FizzBuzzSize + 2,
                          str_buffer.capacity += base_capacity * 2,
                          str_buffer.capacity, str_buffer.free,
                          str_buffer.length, str_buffer.data, temp_buffer)

      size = snprintf(&str_buffer.data[str_buffer.length], str_buffer.free,
                      "%s", "Fizzbuzz, ");
      assert(size > 0 && size < str_buffer.free);

      str_buffer.length += size;
      str_buffer.free = str_buffer.capacity - str_buffer.length;

      continue;
    }

    if (i % 3 == 0) {
      REALLOC_IF_NECESARY(str_buffer.free <= FizzSize + 2,
                          str_buffer.capacity += base_capacity * 2,
                          str_buffer.capacity, str_buffer.free,
                          str_buffer.length, str_buffer.data, temp_buffer)

      size = snprintf(&str_buffer.data[str_buffer.length], str_buffer.free,
                      "%s", "Fizz, ");
      assert(size > 0 && size < str_buffer.free);

      str_buffer.length += size;
      str_buffer.free = str_buffer.capacity - str_buffer.length;

      continue;
    }

    if (i % 5 == 0) {
      REALLOC_IF_NECESARY(str_buffer.free <= BuzzSize + 2,
                          str_buffer.capacity += base_capacity * 2,
                          str_buffer.capacity, str_buffer.free,
                          str_buffer.length, str_buffer.data, temp_buffer)

      size = snprintf(&str_buffer.data[str_buffer.length], str_buffer.free,
                      "%s", "Buzz, ");
      assert(size > 0 && size < str_buffer.free);

      str_buffer.length += size;
      str_buffer.free = str_buffer.capacity - str_buffer.length;

      continue;
    }

    REALLOC_IF_NECESARY(str_buffer.free <= snprintf(NULL, 0, "%ld, ", i),
                        str_buffer.capacity += base_capacity * 2,
                        str_buffer.capacity, str_buffer.free, str_buffer.length,
                        str_buffer.data, temp_buffer)

    size = snprintf(&str_buffer.data[str_buffer.length], str_buffer.free,
                    "%ld, ", i);
    assert(size > 0 && size < str_buffer.free);

    str_buffer.length += size;
    str_buffer.free = str_buffer.capacity - str_buffer.length;
  }

  REALLOC_IF_NECESARY(str_buffer.free <= 4, str_buffer.capacity += 20,
                      str_buffer.capacity, str_buffer.free, str_buffer.length,
                      str_buffer.data, temp_buffer)

  size = snprintf(&str_buffer.data[str_buffer.length - 2], str_buffer.free,
                  "%s", ".");
  assert(size > 0 && size < str_buffer.free);

  return str_buffer.data;
}

long getFizzBuzzString(char *buffer, int capacity, long start, long end) {
  long value_size;
  int write_size;

  long write_offset = 0;

  for (; start <= end; start++) {
    if ((start % 3 == 0) && (start % 5 == 0)) {
      value_size = 11;

      if (capacity < value_size)
        break;

      write_size =
          snprintf(&buffer[write_offset], capacity, "%s", "Fizzbuzz, ");
      assert(write_size > 0 && write_size < capacity);

      capacity -= value_size;
      write_offset += write_size;
    } else if (start % 3 == 0) {
      value_size = 7;

      if (capacity < value_size)
        break;

      write_size = snprintf(&buffer[write_offset], capacity, "%s", "Fizz, ");
      assert(write_size > 0 && write_size < capacity);

      capacity -= value_size;
      write_offset += write_size;
    } else if (start % 5 == 0) {
      value_size = 7;

      if (capacity < value_size)
        break;

      write_size = snprintf(&buffer[write_offset], capacity, "%s", "Buzz, ");
      assert(write_size > 0 && write_size < capacity);

      capacity -= value_size;
      write_offset += write_size;
    } else {
      value_size = snprintf(NULL, 0, "%ld, ", start);

      if (capacity <= value_size)
        break;

      write_size = snprintf(&buffer[write_offset], capacity, "%ld, ", start);
      assert(write_size > 0 && write_size < capacity);

      capacity -= value_size;
      write_offset += write_size;
    }

    if (start != end) {
      continue;
    }

    write_size = snprintf(&buffer[write_offset - 2], capacity + 2, "%s", ".");
    assert(write_size > 0 && write_size < capacity + 2);

    return -1;
  }

  return start;
}
