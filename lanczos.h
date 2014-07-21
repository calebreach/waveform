#pragma once

class Image;

double lanczos(double t);
double lanczosIntegral(double t);

class LanczosRasterizer {
public:
  LanczosRasterizer();
  void drawLine(Image& img, double x, double y1, double y2);
};
