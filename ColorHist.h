#ifndef COLORHIST_H
#define COLORHIST_H

class Color;

int const COLOR_RANGE = 16;

class ColorHist {
 public :
  ColorHist();


  int getColor(Color c) const;

  int getColor(int r, int g, int b) const;

  void addColor(Color c);

 private :
  int rgbSpace[COLOR_RANGE][COLOR_RANGE][COLOR_RANGE]; // Color space, 3-dimensional int array
};

#endif
