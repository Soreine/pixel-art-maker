#include "ColorHist.h"

#include "CImg-1.5.7/CImg.h"
using namespace cimg_library;

ColorHist::ColorHist() {}

int ColorHist::getColor(Color c) const {
  return this->rgbSpace[c.getR()][c.getG()][c.getB()];
}

void ColorHist::addColor(Color c) {
  this->rgbSpace[c.getR()][c.getG()][c.getB()]++;
}
