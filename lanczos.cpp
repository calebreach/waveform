#include "lanczos.h"
#include "image.h"

#include <gsl/gsl_sf.h>
#include <math.h>
#include <assert.h>

double lanczos(double t, int radius) {
  if (t < -radius) t = -radius;
  if (t >  radius) t =  radius;

  return gsl_sf_sinc(t)*gsl_sf_sinc(t/(double)radius);
}

double lanczosIntegral(double t, int radius) {
  if (t < -radius) t = -radius;
  if (t >  radius) t =  radius;

  // Integrate[Sinc[Pi*x]*Sinc[Pi*x/a], {x, -a, t}, 
  //           Assumptions -> -a < t < a && a \[Element] Integers && a > 0]

  return (
        -2*sin(M_PI*t)*gsl_sf_sinc(t/radius)
      + (-1 + radius)*gsl_sf_Si(M_PI - radius*M_PI)
      + ( 1 + radius)*gsl_sf_Si(M_PI + radius*M_PI)
      - (-1 + radius)*gsl_sf_Si(((-1 + radius)*M_PI*t)/radius)
      + ( 1 + radius)*gsl_sf_Si((( 1 + radius)*M_PI*t)/radius)
    )/(2.*M_PI);
}

LanczosRasterizer::LanczosRasterizer(int radius) : radius(radius) {
}

static void getXCoefs(float dx, int radius, float* coefs) {
  int size = radius*2;
  for (int i=0; i<size; i++)
    coefs[i] = lanczos(dx + (i - radius), radius);
}

static void getYCoefs(float dy, int radius, float* coefs) {
  int size = radius*2 + 1;
  double last = 0;
  for (int i=0; i<size; i++) {
    float cur = lanczosIntegral(dy + (i - radius), radius);
    coefs[i] = (float)(cur - last);
    last = cur;
  }
}

void normalizeCoefs(float* coefs, int size) {
  float scale = 0.0;
  for (int i=0; i<size; i++)
    scale += coefs[i];
  scale = 1.0f/scale;
  for (int i=0; i<size; i++)
    coefs[i] *= scale;
}

void drawEndPoint(Image& img, float* xcoefs, int xi, float y, int radius, bool negate) {
  int xsize = radius*2;
  int ysize = radius*2 + 1;

  float ycoefs[radius*2+1];
  int yi = (int)y;
  float dy = yi - y;
  getYCoefs(dy, radius, ycoefs);
  normalizeCoefs(ycoefs, ysize);

  xi -= radius - 1;
  yi -= radius - 1;

  for (int yj=0; yj<ysize; yj++) {
    for (int xj=0; xj<xsize; xj++) {
      float v = xcoefs[xj]*ycoefs[yj];
      img.add(xi+xj, yi+yj, negate ? -v : v);
    }
  }
}

// TODO handle case where y1 ~== y2
void LanczosRasterizer::drawLine(Image& img, float x, float y1, float y2) {
  float scale = 1.0/(y2 - y1);
  int xsize = radius*2;
  float xcoefs[radius*2];
  int xi = (int)x;
  float dx = xi - x;
  getXCoefs(dx, radius, xcoefs);
  normalizeCoefs(xcoefs, xsize);
  for (int i=0; i<xsize; i++)
    xcoefs[i] *= scale;

  drawEndPoint(img, xcoefs, xi, y1, radius, false);
  drawEndPoint(img, xcoefs, xi, y2, radius, true);
}
