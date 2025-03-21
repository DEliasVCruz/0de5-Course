#include <alloca.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PARSE_FAILED 1
#define PARSE_SUCCESS 0

#define PRINT_USAGE(cmd) printf("Usage: %s <number>\n", cmd)

typedef struct {
  int length;
  char *value;
} string;

int parse_int_string(char *str, int *number);
int wordinator_str_size(int number);
void write_wordinator_str(int number, char *buffer, int *buffer_idx,
                          int *capacity);

string number_names[91] = {
    {.length = 4, .value = "zero"},     {.length = 3, .value = "one"},
    {.length = 3, .value = "two"},      {.length = 5, .value = "three"},
    {.length = 4, .value = "four"},     {.length = 4, .value = "five"},
    {.length = 3, .value = "six"},      {.length = 5, .value = "seven"},
    {.length = 5, .value = "eight"},    {.length = 4, .value = "nine"},
    {.length = 3, .value = "ten"},      {.length = 6, .value = "eleven"},
    {.length = 6, .value = "twelve"},   {.length = 8, .value = "thirteen"},
    {.length = 8, .value = "fourteen"}, {.length = 9, .value = "fifthteen"},
    {.length = 7, .value = "sixteen"},  {.length = 9, .value = "seventeen"},
    {.length = 8, .value = "eighteen"}, {.length = 8, .value = "nineteen"},
    {.length = 6, .value = "twenty"}};

string bin_strings[4] = {{.length = 0, .value = NULL},
                         {.length = 9, .value = " thousand"},
                         {.length = 8, .value = " million"},
                         {.length = 8, .value = " billion"}};

int main(int argc, char *argv[]) {
  number_names[30] = (string){.length = 6, .value = "thirty"};
  number_names[40] = (string){.length = 5, .value = "forty"};
  number_names[50] = (string){.length = 7, .value = "fifthty"};
  number_names[60] = (string){.length = 5, .value = "sixty"};
  number_names[70] = (string){.length = 7, .value = "seventy"};
  number_names[80] = (string){.length = 6, .value = "eighty"};
  number_names[90] = (string){.length = 6, .value = "ninety"};

  int parsed_number;
  int parse_status;
  char *bin;

  char *string_buffer;

  int buffer_written;
  int string_capacity;
  int string_buffer_index;

  int string_buffer_size = 1;

  if (argc < 2) {
    fprintf(stderr, "You must pass a number to wordinate\n");
    PRINT_USAGE(argv[0]);

    return 1;
  }

  char *number_str = argv[1];
  int number_str_length = strlen(number_str);

  if (number_str_length <= 0) {
    fprintf(stderr, "You must pass a number to wordinate\n");
    PRINT_USAGE(argv[0]);
  }

  if (number_str_length <= 3) {
    parse_status = parse_int_string(number_str, &parsed_number);

    if (parse_status == PARSE_FAILED) {
      fprintf(stderr, "Pliss use a numeric value\n");
      PRINT_USAGE(argv[0]);

      return 1;
    }

    if (parsed_number == 0) {
      string_buffer_size += number_names[0].length;
      string_buffer = (char *)alloca(string_buffer_size);

      snprintf(string_buffer, string_buffer_size, "%s", number_names[0].value);
      printf("The number is %s\n", string_buffer);

      return 0;
    }

    string_buffer_size += wordinator_str_size(parsed_number);

    string_buffer = (char *)alloca(string_buffer_size);
    string_capacity = string_buffer_size;

    write_wordinator_str(parsed_number, string_buffer, &string_buffer_index,
                         &string_capacity);

    printf("The number is %s\n", string_buffer);

    return 0;
  }

  char buffer[4];
  int buffer_idx = 0;

  int parsed_numbers_length = ceil(number_str_length / 3.);
  int parsed_numbers[parsed_numbers_length];
  int parsed_numbers_idx = 0;

  int i = 0;
  while (i <= number_str_length) {
    if (((number_str_length - i) % 3) != 0 || buffer_idx == 0) {
      snprintf(&buffer[buffer_idx], 4, "%c", number_str[i]);

      buffer_idx++;
      i++;

      continue;
    }

    parse_status = parse_int_string(buffer, &parsed_number);
    if (parse_status == PARSE_FAILED) {
      fprintf(stderr, "Pliss use a numeric value\n");
      PRINT_USAGE(argv[0]);

      return 1;
    }

    parsed_numbers[parsed_numbers_idx] = parsed_number;
    parsed_numbers_idx++;

    buffer_idx = 0;
    if (parsed_number == 0) {
      continue;
    }

    if (string_buffer_size > 1) {
      string_buffer_size += 2;
    }

    int wordinator_size = wordinator_str_size(parsed_number);
    int tst_bin_size = bin_strings[(number_str_length - 1) / 3].length;
    int the_size = wordinator_size + tst_bin_size;

    string_buffer_size += the_size;
  }

  string_buffer = (char *)malloc(string_buffer_size);
  if (string_buffer == NULL) {
    fprintf(stderr, "could not allocate enough memory for string\n");

    return 1;
  }

  string_capacity = string_buffer_size;
  string_buffer_index = 0;

  for (i = 0; i < parsed_numbers_length; i++) {
    parsed_number = parsed_numbers[i];

    if (parsed_number == 0) {
      continue;
      ;
    }

    if (buffer_written > 0) {
      buffer_written = snprintf(&string_buffer[string_buffer_index],
                                string_capacity, "%s", ", ");

      string_buffer_index += buffer_written;
      string_capacity -= buffer_written;
    }

    write_wordinator_str(parsed_number, string_buffer, &string_buffer_index,
                         &string_capacity);

    bin = bin_strings[parsed_numbers_length - (i + 1)].value;
    if (bin == NULL) {
      continue;
    }

    buffer_written = snprintf(&string_buffer[string_buffer_index],
                              string_capacity, "%s", bin);

    string_buffer_index += buffer_written;
    string_capacity -= buffer_written;
  }

  printf("The number is %s\n", string_buffer);
  free(string_buffer);

  return 0;
}

int parse_int_string(char *str, int *number) {
  char *endptr;

  *number = strtol(str, &endptr, 10);

  if ((errno == ERANGE && (*number == INT_MIN || *number == INT_MAX)) ||
      (errno != 0 && *number == 0)) {
    return PARSE_FAILED;
  }

  if (endptr == str) {
    return PARSE_FAILED;
  }

  if (*endptr != '\0') {
    return PARSE_FAILED;
  }

  return PARSE_SUCCESS;
}

int wordinator_str_size(int number) {
  int size = 0;

  if (number == 0) {
    size += number_names[0].length;

    return size;
  }

  int dec_number = number % 100;
  int cent_part = (number - dec_number) / 100;

  int base_part;
  int dec_part;

  if (cent_part > 0) {
    size += number_names[cent_part].length + 8;

    if (dec_number > 0) {
      size += 5;
    }
  }

  if (dec_number > 0 && dec_number <= 20) {
    size += number_names[dec_number].length;
  } else if (dec_number > 0) {
    base_part = dec_number % 10;
    dec_part = (dec_number - base_part);

    size += number_names[dec_part].length;

    if (base_part > 0) {
      size += number_names[base_part].length + 1;
    }
  }

  return size;
}

void write_wordinator_str(int number, char *buffer, int *buffer_idx,
                          int *capacity) {
  int dec_number = number % 100;
  int cent_part = (number - dec_number) / 100;

  int base_part;
  int dec_part;

  int written;

  if (cent_part > 0) {
    written = snprintf(&buffer[*buffer_idx], *capacity, "%s hundred",
                       number_names[cent_part].value);

    *buffer_idx += written;
    *capacity -= written;

    if (dec_number > 0) {
      written = snprintf(&buffer[*buffer_idx], *capacity, "%s", " and ");

      *buffer_idx += written;
      *capacity -= written;
    }
  }

  if (dec_number > 0 && dec_number <= 20) {
    written = snprintf(&buffer[*buffer_idx], *capacity, "%s",
                       number_names[dec_number].value);

    *buffer_idx += written;
    *capacity -= written;
  } else if (dec_number > 0) {
    base_part = dec_number % 10;
    dec_part = (dec_number - base_part);

    written = snprintf(&buffer[*buffer_idx], *capacity, "%s",
                       number_names[dec_part].value);

    *buffer_idx += written;
    *capacity -= written;

    if (base_part > 0) {
      written = snprintf(&buffer[*buffer_idx], *capacity, "-%s",
                         number_names[base_part].value);

      *buffer_idx += written;
      *capacity -= written;
    }
  }
}
