#include <alloca.h>
#include <errno.h>
#include <limits.h>
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

string bin_strings[4] = {{.length = 0, .value = ""},
                         {.length = 9, .value = " thousand"},
                         {.length = 8, .value = " million"},
                         {.length = 8, .value = " billion"}};

int main(int argc, char *argv[]) {
  number_names[50] = (string){.length = 7, .value = "fifthty"};
  number_names[60] = (string){.length = 5, .value = "sixty"};
  number_names[70] = (string){.length = 7, .value = "seventy"};
  number_names[80] = (string){.length = 6, .value = "eighty"};
  number_names[90] = (string){.length = 6, .value = "ninety"};

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
    int number;
    int parse_status = parse_int_string(number_str, &number);

    if (parse_status == PARSE_FAILED) {
      fprintf(stderr, "Pliss use a numeric value\n");
      PRINT_USAGE(argv[0]);

      return 1;
    }

    char *string_buffer;
    int string_buffer_size = 1;

    if (number == 0) {
      string_buffer_size += number_names[0].length;
      string_buffer = (char *)alloca(string_buffer_size);

      snprintf(string_buffer, string_buffer_size, "%s", number_names[0].value);
      printf("The number is %s\n", string_buffer);

      return 0;
    }

    int dec_number = number % 100;
    int cent_part = (number - dec_number) / 100;

    int base_part;
    int dec_part;

    if (cent_part > 0) {
      string_buffer_size += number_names[cent_part].length + 8;

      if (dec_number > 0) {
        string_buffer_size += 5;
      }
    }

    if (dec_number > 0 && dec_number <= 20) {
      string_buffer_size += number_names[dec_number].length;
    } else if (dec_number > 0) {
      base_part = dec_number % 10;
      dec_part = (dec_number - base_part);

      string_buffer_size +=
          number_names[dec_part].length + number_names[base_part].length + 1;
    }

    string_buffer = (char *)alloca(string_buffer_size);
    int string_capacity = string_buffer_size;
    int buffer_index = 0;

    int buffer_written;

    if (cent_part > 0) {
      buffer_written = snprintf(&string_buffer[buffer_index], string_capacity,
                                "%s hundred", number_names[cent_part].value);

      buffer_index += buffer_written;
      string_capacity -= buffer_written;

      if (dec_number > 0) {
        buffer_written = snprintf(&string_buffer[buffer_index], string_capacity,
                                  "%s", " and ");

        buffer_index += buffer_written;
        string_capacity -= buffer_written;
      }
    }

    if (dec_number > 0 && dec_number <= 20) {
      buffer_written = snprintf(&string_buffer[buffer_index], string_capacity,
                                "%s", number_names[dec_number].value);

      buffer_index += buffer_written;
      string_capacity -= buffer_written;
    } else {
      buffer_written =
          snprintf(&string_buffer[buffer_index], string_capacity, "%s-%s",
                   number_names[dec_part].value, number_names[base_part].value);

      buffer_index += buffer_written;
      string_capacity -= buffer_written;
    }

    printf("The number is %s\n", string_buffer);

    return 0;
  }

  short tail_number = number_str_length % 3;

  number_str_length -= tail_number;

  int bins = number_str_length / 3;

  int number;
  int parse_status = parse_int_string(number_str, &number);

  if (parse_status == PARSE_FAILED) {
    fprintf(stderr, "Pliss use a numeric value\n");
    PRINT_USAGE(argv[0]);

    return 1;
  }

  int string_buffer_size = 1;

  int dec_number = number % 100;
  int cent_part = (number - dec_number) / 100;

  int base_part;
  int dec_part;

  if (cent_part > 0) {
    string_buffer_size += number_names[cent_part].length + 8;

    if (dec_number > 0) {
      string_buffer_size += 5;
    }
  }

  if (dec_number > 0 && dec_number <= 20) {
    string_buffer_size += number_names[dec_number].length;
  } else if (dec_number > 0) {
    base_part = dec_number % 10;
    dec_part = (dec_number - base_part);

    string_buffer_size +=
        number_names[dec_part].length + number_names[base_part].length + 1;
  }

  char string_buffer[string_buffer_size];
  int string_capacity = string_buffer_size;
  int buffer_index = 0;

  int buffer_written;

  if (cent_part > 0) {
    buffer_written = snprintf(&string_buffer[buffer_index], string_capacity,
                              "%s hundred", number_names[cent_part].value);

    buffer_index += buffer_written;
    string_capacity -= buffer_written;

    if (dec_number > 0) {
      buffer_written = snprintf(&string_buffer[buffer_index], string_capacity,
                                "%s", " and ");

      buffer_index += buffer_written;
      string_capacity -= buffer_written;
    }
  }

  if (dec_number > 0 && dec_number <= 20) {
    buffer_written = snprintf(&string_buffer[buffer_index], string_capacity,
                              "%s", number_names[dec_number].value);

    buffer_index += buffer_written;
    string_capacity -= buffer_written;
  } else {
    buffer_written =
        snprintf(&string_buffer[buffer_index], string_capacity, "%s-%s",
                 number_names[dec_part].value, number_names[base_part].value);

    buffer_index += buffer_written;
    string_capacity -= buffer_written;
  }

  printf("The number is %s\n", string_buffer);

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
