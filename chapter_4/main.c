#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PRINT_USAGE printf("Usage: %s file\n", argv[0])
#define MALFORMED_BM printf("Malformed bit map file: %s\n", file_name)
#define BM_FILE_HEADER_SIZE 14
#define ASCII_RENDER_TMPL "\033[48;2;%d;%d;%dm \033[0m"

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
  int pixel_row_bytes_length;
  int bytes_per_pixel;
  int row_string_size;
  int bytes_written;
  int byte_idx_end;

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

  pixel_row_bytes_length =
      info_header.pixel_image_width * sizeof(bgb_pixel_t) + byte_padding;

  if (info_header.pixel_image_height * pixel_row_bytes_length !=
      info_header.image_bytes_size) {
    MALFORMED_BM;
    PRINT_USAGE;

    return 1;
  }

  byte_t *pixels =
      calloc(info_header.pixel_image_height, pixel_row_bytes_length);
  assert(pixels != NULL);

  bytes_read = fread(pixels, pixel_row_bytes_length,
                     info_header.pixel_image_height, bm_image_file);
  assert(bytes_read > 0);

  fclose(bm_image_file), bm_image_file = NULL;

  // Add up the number of bytes in the row and the rending string size times
  // the number of pixels in the row
  row_string_size = pixel_row_bytes_length + 22 * info_header.pixel_image_width;

  // Allocate the render string buffer
  render_string.buffer =
      calloc(info_header.pixel_image_height, row_string_size);
  assert(render_string.buffer != NULL);

  // Setup the rest of the render string bufgfer
  render_string.capacity = info_header.pixel_image_height * row_string_size;
  render_string.available = render_string.capacity;
  render_string.length = 0;

  // The basic algorithm for rendering the bm image one byte a time from
  // the top left of the image - down, since the bm image pixels are layed
  // out from bottom left - up, we have to read from the end of the pixels
  // array backwords, one row at a time moving left to right in the pixels
  // of the row
  //
  // To this calculation we iterate over each logical row of the image,
  // starting from 1, and for each row we calculate the byte offset in
  // weach the row pixels would start, this is equal to the total nubmer
  // of bytes of the image, minus the bytes length of a row times the row
  // number
  //
  // This would position us right at the start of the row first byte and
  // after any padding, then we iterate n number of bytes per pixels up to
  // the row bytes length minus padding
  bgb_pixel_t *pixel = {0};
  byte_idx_end = pixel_row_bytes_length - byte_padding;

  for (int row = 1; row <= info_header.pixel_image_height; row++) {
    int row_idx_start =
        info_header.image_bytes_size - (pixel_row_bytes_length * row);

    for (int i = 0; i < byte_idx_end; i += bytes_per_pixel) {
      pixel = (bgb_pixel_t *)&pixels[row_idx_start + i];

      bytes_written = snprintf(&render_string.buffer[render_string.length],
                               render_string.available, ASCII_RENDER_TMPL,
                               pixel->red, pixel->green, pixel->blue);
      assert(bytes_written > 0);

      render_string.length += bytes_written;
      render_string.available -= bytes_written;
    }

    bytes_written = snprintf(&render_string.buffer[render_string.length],
                             render_string.available, "%s", "\n");
    assert(bytes_written > 0);

    render_string.length += bytes_written;
    render_string.available -= bytes_written;
  }

  FREE(pixels);

  printf("%s", render_string.buffer);

  FREE(render_string.buffer);

  return 0;
}
