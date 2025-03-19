#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define PRINT_USAGE(cmd) printf("Usage: %s <final-number>\n", cmd)

#ifdef __GNUC__
typedef int vector_4 __attribute__((vector_size(16)));

// The number of elements has to be a power of two
typedef int vector_128 __attribute__((vector_size(512)));
#endif /* ifdef __GNUC__ */

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "You must pass a number to search to\n");
    PRINT_USAGE(argv[0]);

    return 1;
  }

  char *endptr;
  long end_rage = strtol(argv[1], &endptr, 10);

  if ((errno == ERANGE && (end_rage == INT_MIN || end_rage == INT_MAX)) ||
      (errno != 0 && end_rage == 0)) {

    perror("strtol");

    return 1;
  }

  if (endptr == argv[1]) {
    fprintf(stderr, "Pliss use a numeric value\n");
    PRINT_USAGE(argv[0]);

    return 1;
  }

  if (*endptr != '\0') {
    fprintf(stderr, "Pliss use a numeric value\n");
    PRINT_USAGE(argv[0]);

    return 1;
  }

  switch (end_rage) {
  case 0:
    printf("There are no prime number up to 0\n");
    return 0;
  case 1:
    printf("There is only 1 prime number up to 1\n");
    return 0;
  case 2:
    printf("There are only 2 prime numbers up to 2\n");
    return 0;
  default:
    break;
  }

  int prime_count = 1;

  const int all_numbers_length = end_rage - 1;
  int possible_primes_length = end_rage - 1;

  int all_numbers[all_numbers_length];
  int possible_primes[all_numbers_length];

  for (int i = 0; i < all_numbers_length; i++) {
    all_numbers[i] = i + 2;
    possible_primes[i] = i + 2;
  }

  for (int x = 0; x < all_numbers_length; x++) {
    int current_denominator = all_numbers[x];

    if (all_numbers[x] == 0) {
      continue;
    }

    prime_count++;

    int primes_length = 0;

    for (int y = 0; y < possible_primes_length; y++) {
      int num_value = possible_primes[y];

      if ((num_value % current_denominator) == 0) {
        all_numbers[num_value - 2] = 0;

        continue;
      }

      possible_primes[primes_length] = num_value;

      primes_length++;
    }

    possible_primes_length = primes_length;
  }

  printf("There are %d prime numbers up to %s\n", prime_count, argv[1]);

  return 0;
}
