#ifndef COLORHIST_H
#define COLORHIST_H

#include "Color.h"

#define COLOR_RANGE 256

class ColorHist {
 public :
  ColorHist();


  int getColor(Color c) const;

  void addColor(Color c);

 private :
  int rgbSpace[COLOR_RANGE][COLOR_RANGE][COLOR_RANGE];
};

#endif
