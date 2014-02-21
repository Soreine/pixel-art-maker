#include "ColorHist.h"
#include "CImg-1.5.7/CImg.h"
#include <iostream>

using namespace cimg_library;

ColorHist::ColorHist() {
  rgbSpace = new int[COLOR_RANGE][COLOR_RANGE][COLOR_RANGE];
}

ColorHist::~ColorHist() {
  delete[][][] rgbSpace;
}

int ColorHist::getColor(Color c) const {
  std::cout << c.getR() + 1000*(c.getG() + 1000*c.getB());
  return this->rgbSpace[c.getR()][c.getG()][c.getB()];
}

int ColorHist::getColor(int r, int g, int b) const {
  std::cout << r + 1000*(g + 1000*b);
  return this->rgbSpace[r][g][b];
}

void ColorHist::addColor(Color c) {
  this->rgbSpace[c.getR()][c.getG()][c.getB()]++;
}


