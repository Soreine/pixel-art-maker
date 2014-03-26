#include "Color.h"
#include <math.h>


Color::Color() {}

Color::Color(int rVal, int gVal, int bVal) : r(rVal), g(gVal), b(bVal) {
}

int Color::getR() const {
  return this->r;
}

int Color::getG() const {
  return this->g;
}

int Color::getB() const {
  return this->b;
}

double Color::distance(Color const& c1, Color const& c2) const {
  /*
    typedef struct {
    unsigned char r, g, b;
    } RGB;

    double ColourDistance(RGB e1, RGB e2)
    {
    long rmean = ( (long)e1.r + (long)e2.r ) / 2;
    long r = (long)e1.r - (long)e2.r;
    long g = (long)e1.g - (long)e2.g;
    long b = (long)e1.b - (long)e2.b;
    return sqrt((((512+rmean)*r*r)>>8) + 4*g*g + (((767-rmean)*b*b)>>8));
    }
  */

  long rmean = ((long) c1.getR() + (long) c2.getR())/2;
  long r = (long) c1.getR() - (long) c2.getR();
  long g = (long) c1.getG() - (long) c2.getG();
  long b = (long) c1.getB() - (long) c2.getB();

  return sqrt((((512 + rmean)*r*r)>>8) + 4<<g*g + (((767-rmean)*b*b)>>8));
}
