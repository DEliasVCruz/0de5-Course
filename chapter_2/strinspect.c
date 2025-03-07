#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
  if (argc <= 1) {
    puts("No string to inspect provided");

    return 0;
  }

  char *input = argv[1];
  size_t input_length = strlen(input);

  if (input_length == 0) {
    puts("No string to inspect provided");

    return 0;
  }

  // Pre allocate the buffer on the stack
  char buffer[4 * ((input_length * 9) + 8)];

  int line_offset = (input_length * 9) + 7;

  int ascci_start = 0 + (line_offset) * 0;
  int hex_start = 1 + (line_offset) * 1;
  int dec_start = 2 + (line_offset) * 2;
  int bin_start = 3 + (line_offset) * 3;

  strncpy(&(buffer[ascci_start]), "ASCII:", 7);
  strncpy(&(buffer[hex_start]), "Hex:  ", 7);
  strncpy(&(buffer[dec_start]), "Dec:  ", 7);
  strncpy(&(buffer[bin_start]), "Bin:  ", 7);

  buffer[sizeof(buffer) - 1] = '\0';

  for (size_t i = 0; i < input_length; i++) {
    snprintf(&(buffer[ascci_start + 6 + (9 * i)]), 11, " %8c\n", input[i]);
    snprintf(&(buffer[hex_start + 6 + (9 * i)]), 11, " %8x\n", input[i]);
    snprintf(&(buffer[dec_start + 6 + (9 * i)]), 11, " %8d\n", input[i]);

    buffer[bin_start + 6 + (9 * i)] = ' ';
    for (int ibit = 7, offset = 1; ibit >= 0; ibit--, offset++) {
      char bit = input[i] & 1 << ibit;

      snprintf(&(buffer[bin_start + 6 + (9 * i) + offset]), 3, "%d\n",
               bit >> ibit);
    }
  }

  printf("%s", &(buffer[ascci_start]));
  printf("%s", &(buffer[hex_start]));
  printf("%s", &(buffer[dec_start]));
  printf("%s", &(buffer[bin_start]));

  return 0;
}
