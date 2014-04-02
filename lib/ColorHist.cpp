#include "ColorHist.h"
#include "Color.h"
#include <iostream>

using namespace std;

ColorHist::ColorHist() {
  rgbSpace = new tab2d[COLOR_RANGE];
}


ColorHist::~ColorHist() {
  delete[] rgbSpace;
}


unsigned int ColorHist::getColor(Color c) const {
  return this->rgbSpace[c.getR()][c.getG()][c.getB()];
}

unsigned int ColorHist::getColor(unsigned char r, unsigned  char g, unsigned  char b) const {
  return this->rgbSpace[r][g][b];
}

bool ColorHist::addColor(Color c) {
  return this->rgbSpace[c.getR()][c.getG()][c.getB()]++;
}


