#ifndef COLORHIST_H
#define COLORHIST_H

#include <map>

class Color;

int const COLOR_RANGE = 256;

typedef int tab2d[COLOR_RANGE][COLOR_RANGE];
class ColorHist {
 public :
  ColorHist();
  
  ~ColorHist();

  int getColor(Color c) const;
  
  int getColor(int r, int g, int b) const;
  
  void addColor(Color c);
  
 private :
  // Map to store number of pixel of each color.
  tab2d *rgbSpace;
};

#endif
