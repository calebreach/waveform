#pragma once

#include <string>

class Image {
  float* data;
  int width, height;

public:
  Image(int width, int height);
  virtual ~Image();
  void integrateY();
  void savePNG(const std::string& path);
  void add(int x, int y, float amount) {
    if (x < 0 || y < 0 || x >= width || y >= height)
      return;

    int ix = y*width + x;
    data[ix] += amount;
  }
};
