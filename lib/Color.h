#ifndef COLOR_H
#define COLOR_H

#include <ostream>
#include "Triplet.h"

class Color {
 public :
  /** Create a non initialized Color object */
  Color();

  /** Create a Color with the specified RGB values */
  Color(unsigned char r, unsigned char g, unsigned char b);

  unsigned char getR() const;
  unsigned char getG() const;
  unsigned char getB() const;

  /** Return a Triplet representing this color */
  Triplet toTriplet();

  /** Return a color made from the given amount of 'c1' and (1 -
      amount) of c2 */
  static Color mix(Color const& c1, Color const& c2, float amount);

  /** Return the euclidian distance between two colors */
  static double distance(Color const& c1, Color const& c2);

  /** Return the squared euclidian distance between two colors */
  static long distance2(Color const& c1, Color const& c2);

  /** Return a more human-eye accurate distance between two colors. */
  static double colorCompare(Color const& c1, Color const& c2);

 private :
  /** The three RGB components */
  unsigned char r;
  unsigned char g;
  unsigned char b;
};

// To stream
std::ostream & operator<<(std::ostream &, const Color &);

#endif
