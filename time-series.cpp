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
  // rast.drawLine(img,10,20,30);
  // for (int i=0; i<10; i++)
  //   rast.drawLine(img, 4 + i*9, 20, 20 + i*0.5f);

  rast.drawLine(img, 30, 20, 20);
  rast.drawLine(img, 50, 20, 25);
  rast.drawLine(img, 60, 20, 30);

  // for (int i=0; i<10; i++)
  //     rast.drawLine(img, 4 + i*9, 20, 20 + i*0.04);


  img.integrateY();
  img.savePNG("out.png");
}
