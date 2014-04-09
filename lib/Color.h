#ifndef COLOR_H
#define COLOR_H

#include <ostream>

class Color {
 public :
  Color();

  Color(unsigned char r, unsigned char g, unsigned char b);

  unsigned char getR() const;
  unsigned char getG() const;
  unsigned char getB() const;

  Color mix(Color const& c, float amount);
  
  static double distance(Color const& c1, Color const& c2);

 private :
  unsigned char r;
  unsigned char g;
  unsigned char b;
};

std::ostream & operator<<(std::ostream &, const Color &);

#endif
