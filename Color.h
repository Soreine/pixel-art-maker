#ifndef COLOR_H
#define COLOR_H

class Color {
 public :
  Color();

  Color(unsigned char r, unsigned char g, unsigned char b);

  unsigned char getR() const;
  unsigned char getG() const;
  unsigned char getB() const;
  
  double distance(Color const& c1, Color const& c2) const;

 private :
  unsigned char r;
  unsigned char g;
  unsigned char b;
};

#endif
