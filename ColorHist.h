#ifndef COLORHIST_H
#define COLORHIST_H

#include <map>

class Color;

unsigned int const COLOR_RANGE = 256;

typedef unsigned int tab2d[COLOR_RANGE][COLOR_RANGE];

class ColorHist {
 public :
  ColorHist();
  
  ~ColorHist();

  unsigned int getColor(Color c) const;
  
  unsigned int getColor(unsigned char r, unsigned char g, unsigned char b) const;
  
  void addColor(Color c);
  
 private :
  // Map to store number of pixel of each color.
  tab2d *rgbSpace;
};

#endif
