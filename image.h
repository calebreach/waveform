#pragma once

#include <string>

class Image {
  float* data;
  int width, height;

public:
  Image(int width, int height);
  virtual ~Image();

  int getSize() { return width*height; }
  int getWidth() { return width; }
  int getHeight() { return height; }

  void integrateY();
  void integrateYToMiddle();
  void log(float cutoff);
  void sqrt();
  void clip(float low, float high);
  float quantile(float percent);
  void normalize();
  void savePNG(const std::string& path);
  void add(int x, int y, float amount) {
    if (x < 0 || y < 0 || x >= width || y >= height)
      return;

    int ix = y*width + x;
    data[ix] += amount;
  }
};
