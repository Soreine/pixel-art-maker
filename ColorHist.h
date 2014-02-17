#ifndef COLORHIST_H
#define COLORHIST_H

class ColorHist {
 public :
  ColorHist();

  ColorHist(char* file_image);

  int getColor(Color c) const;

  void addColor(Color c);

 private :
  int[][][] rgbSpace;
};

#endif
