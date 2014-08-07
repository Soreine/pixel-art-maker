#include "Color.h"
#include "Triplet.h"
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

  return sqrt((((512 + rmean)*r*r)>>8) + (4<<g*g) + (((767-rmean)*b*b)>>8));
  */

  return sqrt(distance2(c1, c2));

}

double Color::distance2(Color const& c1, Color const& c2){
  long r = (long) c1.getR() - (long) c2.getR();
  long g = (long) c1.getG() - (long) c2.getG();
  long b = (long) c1.getB() - (long) c2.getB();
  return (r*r + g*g + b*b);
}

double Color::colorCompare(Color const& c1, Color const& c2)
{
  int r1 = c1.getR();
  int g1 = c1.getG();
  int b1 = c1.getB();
  int r2 = c2.getR();
  int g2 = c2.getG();
  int b2 = c2.getB();
  double luma1 = (r1*299 + g1*587 + b1*114) / (255.0*1000);
  double luma2 = (r2*299 + g2*587 + b2*114) / (255.0*1000);
  double lumadiff = luma1-luma2;
  double diffR = (r1-r2)/255.0, diffG = (g1-g2)/255.0, diffB = (b1-b2)/255.0;
  return (diffR*diffR*0.299 + diffG*diffG*0.587 + diffB*diffB*0.114)*0.75
    + lumadiff*lumadiff;
}


Color Color::mix(Color const& c1, Color const& c2, float amount){
  /* 
     assert(amount <= 1);
     assert(amount >= 0);
  */

  Color result(round(c1.getR()*amount + c2.getR()*(1-amount)),
	       round(c1.getG()*amount + c2.getG()*(1-amount)),
	       round(c1.getB()*amount + c2.getB()*(1-amount)));
  return result;
}
  
// Multiply the component of this color by the
void multiply(unsigned int m) {
  this->r*=m;
  this->g*=m;
  this->b*=m;
}

Triplet toTriplet() {
  return Triplet(this->r, this->g, this->b);
}

std::ostream& operator<<(std::ostream &strm, const Color &c) {
  return strm << "Color(" 
	      << (int) c.getR() << ","
	      << (int) c.getG() << "," 
	      <<  (int) c.getB() << ")";
}
