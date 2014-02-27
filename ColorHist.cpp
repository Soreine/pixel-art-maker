#include "ColorHist.h"
#include "Color.h"
#include "CImg-1.5.7/CImg.h"
#include <iostream>

using namespace cimg_library;

ColorHist::ColorHist() {
  // rgbSpace = new ;
  
  // tab[10]
  // *(tab+10)
  // tab=(int*)malloc(nb_elements*sizeof(int));

  /* The C way
  rgbSpace = (int***) malloc(COLOR_RANGE*sizeof(int**));  
  for(int i=0; i < COLOR_RANGE; i++) {
    rgbSpace[i] = (int**) malloc(COLOR_RANGE*sizeof(int*));
    for(int j=0; j < COLOR_RANGE; j++) {
      rgbSpace[i][j] = (int*) malloc(COLOR_RANGE*sizeof(int));
    }
  }
  */
  
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


