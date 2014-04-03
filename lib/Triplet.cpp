#include "Triplet.h"
#include "Color.h"
#include <math.h>
#include <ostream>

Triplet::Triplet() {}

Triplet::Triplet(long x, long y, long z) {
  values[0] = x;
  values[1] = y;
  values[2] = z;
}

void Triplet::add(Triplet const& t) {
  values[0] += t.values[0];
  values[1] += t.values[1];
  values[2] += t.values[2];
}


void Triplet::addMultiply(Color const& c, unsigned int m) {
  values[0] += m*c.getR();
  values[1] += m*c.getG();
  values[2] += m*c.getB();
}

void Triplet::divide(long d) {
  values[0] = lround((double)values[0]/(double)d);
  values[1] = lround((double)values[1]/(double)d);
  values[2] = lround((double)values[2]/(double)d);
}

Color Triplet::getColor() const {

}
