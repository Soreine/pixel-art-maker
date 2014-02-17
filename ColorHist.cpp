#include "ColorHist.h"

#include "CImg.h"
using namespace cimg_library;

ColorHist::ColorHist() {}

ColorHist::ColorHist(char* file_image) {
}

int ColorHist::getColor(Color c) const {
  return this->rgbSpace[c.getR()][c.getG()][c.getB()];
}

void ColorHist::addColor(Color c) const {
  this->rgbSpace[c.getR()][c.getG()][c.getB()]++;
}
