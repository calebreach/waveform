#pragma once

class Image;

double lanczos(double t, int radius=3);
double lanczosIntegral(double t, int radius=3);

class LanczosRasterizer {
  int radius;

public:
  LanczosRasterizer(int radius);
  void drawLine(Image& img, float x, float y1, float y2, float period);
};
