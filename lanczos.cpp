#include "lanczos.h"
#include "image.h"

#include <gsl/gsl_sf.h>
#include <math.h>
#include <assert.h>


#include <iostream>
using namespace std;


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

// TODO refactor
enum EndPointMode {
  integratedEndPoint,
  integratedNegatedEndPoint,
  deltaEndPoint
};

static void getYCoefs(float dy, int radius, float* coefs, EndPointMode mode) {
  int size = radius*2 + 1;
  double last = 0;
  for (int i=0; i<size; i++) {
    double pos = dy + (i - radius);
    double cur;
    switch (mode) {
      case integratedEndPoint:
        cur = lanczosIntegral(pos, radius);
        break;

      case integratedNegatedEndPoint:
        cur = -lanczosIntegral(pos, radius);
        break;

      case deltaEndPoint:
        cur = lanczos(pos, radius);
        break;
    }
    coefs[i] = (float)(cur - last);
    cout << coefs[i] << endl;
    last = cur;
  }
}

void normalizeCoefs(float* coefs, int size) {
  float scale = 0.0;
  for (int i=0; i<size; i++)
    scale += coefs[i];
  scale = 1.0f/fabs(scale);
  for (int i=0; i<size; i++)
    coefs[i] *= scale;
}

void drawEndPoint(Image& img, float* xcoefs, int xi, float y, int radius, EndPointMode mode) {
  int xsize = radius*2;
  int ysize = radius*2 + 1;

  float ycoefs[radius*2+1];
  int yi = (int)y;
  float dy = yi - y;
  getYCoefs(dy, radius, ycoefs, mode);
  normalizeCoefs(ycoefs, ysize);

  xi -= radius - 1;
  yi -= radius - 1;

  for (int yj=0; yj<ysize; yj++) {
    for (int xj=0; xj<xsize; xj++) {
      float v = xcoefs[xj]*ycoefs[yj];
      img.add(xi+xj, yi+yj, v);
    }
  }
}

// FullSimplify[
//  Solve[Variance[UniformDistribution[{y1, y2}]] == 
//    Variance[
//     ProbabilityDistribution[(DiracDelta[t - (y1 + a)] + 
//         DiracDelta[t - (y2 - a)])/2, {t, -\[Infinity], \[Infinity]}]],
//    a, Reals], y1 < y2]
static const float deltaOffsetFactor = (1.0f/6.0f)*(3.0f - sqrtf(3.0f));

// TODO handle case where y1 ~== y2
void LanczosRasterizer::drawLine(Image& img, float x, float y1, float y2) {
  float scale;
  EndPointMode mode1, mode2;

  if (y2 - y1 < 0.1f) {
    mode1 = mode2 = deltaEndPoint;
    scale = 0.5f;
    float offset = (y2 - y1)*deltaOffsetFactor;
    y1 += offset;
    y2 -= offset;
  } else {
    mode1 = integratedEndPoint;
    mode2 = integratedNegatedEndPoint;
    scale = 1.0f/(y2 - y1);
  }

  int xsize = radius*2;
  float xcoefs[radius*2];
  int xi = (int)x;
  float dx = xi - x;
  getXCoefs(dx, radius, xcoefs);
  normalizeCoefs(xcoefs, xsize);
  for (int i=0; i<xsize; i++)
    xcoefs[i] *= scale;

  drawEndPoint(img, xcoefs, xi, y1, radius, mode1);
  drawEndPoint(img, xcoefs, xi, y2, radius, mode2);
}
