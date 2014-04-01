#include "ColorHist.h"
#include "Color.h"
#include "CImg-1.5.7/CImg.h"
#include <iostream>

using namespace cimg_library;
using namespace std;

ColorHist::ColorHist() {
  rgbSpace = new tab2d[COLOR_RANGE];
}


ColorHist::~ColorHist() {
  delete[] rgbSpace;
}


int ColorHist::getColor(Color c) const {
  return this->rgbSpace[c.getR()][c.getG()][c.getB()];
}

int ColorHist::getColor(int r, int g, int b) const {
  return this->rgbSpace[r][g][b];
}

void ColorHist::addColor(Color c) {
  this->rgbSpace[c.getR()][c.getG()][c.getB()]++;
}


