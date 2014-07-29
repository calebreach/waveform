#include <iostream>
#include <sndfile.hh>
#include <math.h>
#include <boost/program_options.hpp>

#include "image.h"
#include "lanczos.h"

using namespace std;

namespace po = boost::program_options;

int main(int argc, char** argv) {
  int width, height;
  string in_path, out_path;

  po::options_description desc("Allowed options");

  desc.add_options()
    ("help", "show help")
    ("width,w", po::value<int>(&width)->default_value(1600), "image width")
    ("height,h", po::value<int>(&height)->default_value(480), "image height")
    ("out,o", po::value<string>(&out_path)->required(), "png output")
    ("input-file", po::value<string>(&in_path)->required(), "audio input");

  po::positional_options_description p;
  p.add("input-file", 1);

  po::variables_map vm;

  try {
    po::store(po::command_line_parser(argc, argv)
                .options(desc).positional(p).run(),
              vm);
    po::notify(vm);
  } catch(po::error& e) {
    std::cerr << "Error: " << e.what() << std::endl << std::endl; 
    std::cerr << desc << std::endl; 
    return 1; 
  }

  if (vm.count("help")) {
    cerr << desc << endl;
    return 1;
  }

  if (width < 0 || height < 0) {
    cerr << "width and height should be positive" << endl;
    return 1;
  }

  Image img(width, height);
  LanczosRasterizer rast(3);

  SndfileHandle sndfile(in_path);
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
  // float low = img.quantile(0.02);
  // float high = img.quantile(0.98);
  // img.log(0.1);
  img.clip(0,INFINITY);
  img.normalize();
  // img.sqrt();
  img.savePNG(out_path);
}
