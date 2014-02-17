#include "Color.h"

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
