#include "image.h"
#include "colormap.h"
#include <assert.h>
#include <png.h>

using namespace std;

Image::Image(int width, int height) : width(width), height(height) {
  int size = width*height;
  data = new float[size];
  fill_n(data, size, 0);
}

Image::~Image() {
  delete data;
}

static uint16_t quantize(float value) {
  int v = ((1<<16)*value);
  if (v > UINT16_MAX) return UINT16_MAX;
  if (v < 0) return 0;
  return (uint16_t)v;
}

static void setComponent(png_byte* data, float value) {
  uint16_t v = quantize(value);
  data[0] = v >> 8;
  data[1] = v & 0xFF;
}

static const int bytes_per_pixel = 6;

static void dataToPixels(float* data, png_byte* pixels, int size,
                         float scale, float min)
{
  for (int i=0; i<size; i++) {
    int pixel_index = i*bytes_per_pixel;
    float r, g, b;
    colormap(scale*(data[i] - min), r, g, b);
    setComponent(pixels + pixel_index,     r);
    setComponent(pixels + pixel_index + 2, g);
    setComponent(pixels + pixel_index + 4, b);
  }
}

void Image::savePNG(const string& path) {
  float min, scale;
  {
    int size = width*height;
    auto minmax_pair = minmax_element(data, data + size);
    min = *minmax_pair.first;
    scale = *minmax_pair.second - min;
    if (scale != 0.0f) scale = 1.0f/scale;
  }

  FILE* fp = fopen(path.c_str(), "wb");
  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  assert(png_ptr);
  assert(!setjmp(png_jmpbuf(png_ptr)));

  png_infop info_ptr = png_create_info_struct(png_ptr);
  assert(info_ptr);

  png_init_io(png_ptr, fp);
  png_set_IHDR(png_ptr, info_ptr, width, height,
               16, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  png_write_info(png_ptr, info_ptr);

  png_byte* pixel_data = new png_byte[width*bytes_per_pixel];
  for (int y=0; y<height; y++) {
    dataToPixels(data + width*y, pixel_data, width, scale, min);
    png_write_row(png_ptr, pixel_data);
  }

  delete pixel_data;

  png_write_end(png_ptr, info_ptr);
  png_destroy_write_struct(&png_ptr, &info_ptr);

  fclose(fp);
}
