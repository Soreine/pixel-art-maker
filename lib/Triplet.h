#ifndef TRIPLET_H
#define TRIPLET_H

class Color;

/** This class represent a three coordinate entity. */
class Triplet {
 public :
  /** Stores the 3 coordinates of this Triplet  */
  long values[3];

  Triplet();

  /** Create a Triplet with the given coordinates */
  Triplet(long x, long y, long z);

  /** Add each relative coordinates of the given Triplet to this one. */
  void add(Triplet const& t);

  /** Add the R,G,B components of 'c' multiplied by 'm' to the x, y, z
      values of this Triplet */
  void multiply(unsigned int m);

  /** Divide all the values by 'd' */
  void divide(long d);
  
  /** Interpret this Triplet as a color */
  Color getColor() const;

};

#endif
