#ifndef POINT_H
#define POINT_H

Class ColorHist {
 public :
  ColorHist(char* file_image);

  int getColor(int r, int g, int b);

  void addColor(int r, int g, int b);

 private :
  vector < vector < vector <int> > > rgbSpace;
}

#endif
