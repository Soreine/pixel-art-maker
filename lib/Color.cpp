#include "Color.h"
#include <math.h>
#include <ostream>

Color::Color() {}

Color::Color(unsigned char rVal, unsigned char gVal, unsigned char bVal) : r(rVal), g(gVal), b(bVal) {
}

unsigned char Color::getR() const {
  return this->r;
}

unsigned char Color::getG() const {
  return this->g;
}

unsigned char Color::getB() const {
  return this->b;
}

double Color::distance(Color const& c1, Color const& c2){
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

  /*
  long rmean = ((long) c1.getR() + (long) c2.getR())/2;
  long r = (long) c1.getR() - (long) c2.getR();
  long g = (long) c1.getG() - (long) c2.getG();
  long b = (long) c1.getB() - (long) c2.getB();

  return sqrt((((512 + rmean)*r*r)>>8) + 4<<g*g + (((767-rmean)*b*b)>>8));
  */

  long r = (long) c1.getR() - (long) c2.getR();
  long g = (long) c1.getG() - (long) c2.getG();
  long b = (long) c1.getB() - (long) c2.getB();

  return sqrt(r*r + g*g + b*b);

}


std::ostream& operator<<(std::ostream &strm, const Color &c) {
  return strm << "Color(" << (int) c.getR() << "," << (int) c.getG() << "," <<  (int) c.getB() << ")";
}
