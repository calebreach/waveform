#include <iostream>
#include <sndfile.hh>
#include <math.h>
#include "image.h"
#include "lanczos.h"

using namespace std;

int main() {
  int width = 1600;
  int height = 480;
  Image img(width, height);
  LanczosRasterizer rast(3);

  SndfileHandle sndfile("in-dnb.flac");
  if (!sndfile) {
    cerr << "couldn't find input file" << endl;
    return 1;
  }
  const int numChannels = sndfile.channels();
  const int size = sndfile.frames()*numChannels;
  const int bufSize = sndfile.samplerate()*numChannels;
  float* buffer = new float[bufSize];
  int numRead;
  float last = 0;
  int start = 0;
  float period = width/(float)sndfile.frames();
  while ((numRead = sndfile.read(buffer, bufSize))) {
    cout << "second" << endl;
    for (int i=0; i<numRead; i+=numChannels) {
      float pos = (start+i)/(float)size;
      float cur = 0.5f*height + 0.4f*height*buffer[i];
      rast.drawLine(img, pos*width, last, cur, period);
      last = cur;
    }
    start += numRead;
  }
  delete buffer;

  // float last = 0;
  // for (int i=0; i<10000; i++) {
  //   float pos = i/10000.0f;
  //   // float cur = sin(pos*M_PI*2*20);
  //   float cur = (i & (1 << 1)) ? 1 : -1;
  //   const float scale = 30;
  //   float y1 = 50 + last*scale;
  //   float y2 = 50 + cur*scale;
  //   rast.drawLine(img, 10 + pos*80, y1, y2);
  //   last = cur;
  // }

  img.integrateY();
  img.integrateYToMiddle();
  float low = img.quantile(0.02);
  float high = img.quantile(0.98);
  // img.log(0.1);
  img.clip(0,INFINITY);
  img.normalize();
  // img.sqrt();
  img.savePNG("out.png");
}
