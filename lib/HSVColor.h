#ifndef HSVCOLOR_H
#define HSVCOLOR_H

#include "Color.h"


typedef struct {
    double h;       // angle in degrees
    double s;       // percent
    double v;       // percent
} hsvComponents;

class HSVColor {
 public :

    /**  The HSV components */
    hsvComponents hsv;

    /** Create a non initialized HSVColor object */
    HSVColor();

    /** Create an HSVColor from the input Color */
    HSVColor(Color const& c);


    /** Return the symmetric color of c about center, constrained to
	the HSV space. */
    static Color getSymmetric(Color const& c, Color const& center);

    /** Return this HSVColor as an RGB Color */
    Color toRGBColor() const;
};

#endif
