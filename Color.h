#ifndef COLOR_H
#define COLOR_H

class Color {
 public :
  Color();

  Color(int r, int g, int b);

  int getR() const;
  int getG() const;
  int getB() const;
  
  double distance(Color const& c1, Color const& c2) const;

 private :
  int r;
  int g;
  int b;
};

#endif
