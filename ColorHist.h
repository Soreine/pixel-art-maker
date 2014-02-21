#ifndef COLORHIST_H
#define COLORHIST_H

#include "Color.h"

#define COLOR_RANGE 256

class ColorHist {
 public :
  ColorHist();


  int getColor(Color c) const;

  int getColor(int r, int g, int b) const;

  void addColor(Color c);

 private :
  int* rgbSpace; // Color space, 3-dimensional int array
};

#endif
