#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PRINT_USAGE printf("Usage: %s file\n", argv[0])
#define MALFORMED_BM printf("Malformed bit map file: %s\n", file_name)
#define BM_FILE_HEADER_SIZE 14

#define FREE(memory) free(memory), memory = NULL

#define OFF_BITS_OFFSET 8

typedef unsigned char byte_t;

typedef struct {
  byte_t blue, green, red;
} bgb_pixel_t;

typedef struct {
  int header_size, pixel_image_width, pixel_image_height;
  short planes, bits_per_pixel;
  int compression, image_bytes_size;
} bm_info_header;

typedef struct {
  int capacity, length, available;
  char *buffer;
} string;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    PRINT_USAGE;

    return 0;
  }

  char *file_name = argv[1];
  char bf_type[2 + 1] = {0};

  int pixel_data_offset;
  size_t bytes_read;
  int byte_padding;
  int pixel_bytes_per_row;
  int pixel_row_bytes_width;
  int bytes_per_pixel;
  int pixels_length;
  int row_string_size;

  string render_string;
  bm_info_header info_header;

  FILE *bm_image_file = fopen(file_name, "rb");

  bytes_read = fread(bf_type, sizeof(char), 2, bm_image_file);
  assert(bytes_read > 0);

  if (strncmp(bf_type, "BM", 2) != 0) {
    MALFORMED_BM;
    PRINT_USAGE;

    return 1;
  }

  if (fseek(bm_image_file, OFF_BITS_OFFSET, SEEK_CUR) != 0) {
    MALFORMED_BM;
    PRINT_USAGE;

    return 1;
  }

  bytes_read = fread(&pixel_data_offset, sizeof(int), 1, bm_image_file);
  assert(bytes_read > 0);

  bytes_read = fread(&info_header, sizeof(bm_info_header), 1, bm_image_file);
  assert(bytes_read > 0);

  if ((BM_FILE_HEADER_SIZE + info_header.header_size) != pixel_data_offset) {
    MALFORMED_BM;
    PRINT_USAGE;

    return 1;
  }

  if (fseek(bm_image_file, pixel_data_offset, SEEK_SET) != 0) {
    MALFORMED_BM;
    PRINT_USAGE;

    return 1;
  }

  bytes_per_pixel = info_header.bits_per_pixel / 8;

  pixel_bytes_per_row = info_header.pixel_image_width * bytes_per_pixel;

  byte_padding =
      pixel_bytes_per_row % 4 == 0 ? 0 : (4 - (pixel_bytes_per_row % 4));

  pixel_row_bytes_width =
      info_header.pixel_image_width * sizeof(bgb_pixel_t) + byte_padding;

  if (info_header.pixel_image_height * pixel_row_bytes_width !=
      info_header.image_bytes_size) {
    MALFORMED_BM;
    PRINT_USAGE;

    return 1;
  }

  byte_t *pixels =
      calloc(info_header.pixel_image_height, pixel_row_bytes_width);
  assert(pixels != NULL);

  bytes_read = fread(pixels, pixel_row_bytes_width,
                     info_header.pixel_image_height, bm_image_file);
  assert(bytes_read > 0);

  fclose(bm_image_file);

  row_string_size = pixel_row_bytes_width + 22 * info_header.pixel_image_width;
  render_string.buffer =
      calloc(info_header.pixel_image_height, row_string_size);
  assert(render_string.buffer != NULL);

  render_string.capacity = info_header.pixel_image_height * row_string_size;
  render_string.available = render_string.capacity;
  render_string.length = 0;

  pixels_length = info_header.pixel_image_height * pixel_row_bytes_width;
  bgb_pixel_t *pixel = {0};
  int bytes_written;

  for (int i = pixels_length - bytes_per_pixel - byte_padding; i >= 0;
       i -= bytes_per_pixel) {
    pixel = (bgb_pixel_t *)&pixels[i];

    bytes_written = snprintf(
        &render_string.buffer[render_string.length], render_string.available,
        "\033[48;2;%d;%d;%dm \033[0m", pixel->red, pixel->green, pixel->blue);
    assert(bytes_written > 0);

    render_string.length += bytes_written;
    render_string.available -= bytes_written;

    if (i == 0 || i % pixel_bytes_per_row == 0) {
      bytes_written = snprintf(&render_string.buffer[render_string.length],
                               render_string.available, "%s", "\n");
      assert(bytes_written > 0);

      render_string.length += bytes_written;
      render_string.available -= bytes_written;

      i -= byte_padding;
    }
  }

  FREE(pixels);

  printf("%s", render_string.buffer);

  FREE(render_string.buffer);

  return 0;
}
