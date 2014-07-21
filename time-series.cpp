#include <iostream>

#include "image.h"

using namespace std;

int main() {
  Image img(100, 100);
  for (int y=0; y<100; y++)
    for (int x=0; x<100; x++)
      img.add(x,y,x);
  img.savePNG("out.png");
}
