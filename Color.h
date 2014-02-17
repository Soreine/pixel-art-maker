#ifndef COLOR_H
#define COLOR_H

class Color {
 public :
  Color();

  Color(int r, int g, int b);

  int getR() const;
  int getG() const;
  int getB() const;

 private :
  int r;
  int g;
  int b;
};

#endif
