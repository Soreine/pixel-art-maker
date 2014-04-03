#ifndef TRIPLET_H
#define TRIPLET_H

class Color;

class Triplet {
 public :
  long values[3];

  Triplet();

  Triplet(long x, long y, long z);

  void add(Triplet const& t);

  void addMultiply(Color const& c, unsigned int m);

  void divide(long d);

  Color getColor() const;

};

#endif
