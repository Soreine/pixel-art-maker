#ifndef HSVCOLOR_H
#define HSVCOLOR_H

#include "Color.h"

#ifndef ABS
#define ABS(A)    ((A) < 0 ? (-(A)) : (A))
#endif

#ifndef MIN
#define MIN(a,b)  ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a,b)  ((a) > (b) ? (a) : (b))
#endif

#ifndef TRIM
#define TRIM(x,a,b) (FA_MIN(FA_MAX(x,a),b))
#endif

typedef struct
{
    double h;       // angle in degrees
    double s;       // percent
    double v;       // percent
} hsvComponents;

class HSVColor
{
public :

    /**  The HSV components */
    hsvComponents hsv;

    /** Create a non initialized HSVColor object */
    HSVColor();

    /** Create an HSVColor from the input Color */
    HSVColor(Color const& c);

    /** Return this HSVColor as an RGB Color */
    Color toRGBColor() const;
};

#endif
