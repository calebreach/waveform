#include <iostream>

#include "image.h"
#include "lanczos.h"

using namespace std;

static void test_lanczos_fns() {
  cout << lanczos(2.4234) << endl;
  cout << lanczos(1.4234, 2) << endl;

  cout << lanczosIntegral(2.4234) << endl;
  cout << lanczosIntegral(1.4234, 2) << endl;
}

static void test_image() {
 Image img(100, 100);
  img.add(15, 20, 0.5f);
  img.add(40, 10, 1.0f);
  img.add(60, 70, 1.5f);
}

static void test_lanczos_rast_x(Image& img) {
  LanczosRasterizer rast(3);
  for (int i=0; i<10; i++)
    rast.drawLine(img, 40 + i*0.2, 4 + i*9, 10 + i*9);
}

static void test_lanczos_rast_y(Image& img) {
  LanczosRasterizer rast(3);
  for (int i=0; i<10; i++)
    rast.drawLine(img, 4 + i*9, 20 + i*0.2, 50 + i*0.2);
}

int main() {
  Image img(100, 100);

  LanczosRasterizer rast(3);
  int n = 44100;
  for (float i=0; i<n; i++)
    rast.drawLine(img, i/(float)n, 10, 50);

  img.integrateY();
  img.savePNG("out.png");
}
