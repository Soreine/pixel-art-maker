#ifndef COLORHIST_H
#define COLORHIST_H

#include <map>

class Color;

unsigned int const COLOR_RANGE = 256;

typedef unsigned int tab2d[COLOR_RANGE][COLOR_RANGE];

/** Can hold a count for the occurrence of each color in an image */
class ColorHist
{
public :
    ColorHist();

    ~ColorHist();

    /** Get the count for the given color */
    unsigned int getColor(Color c) const;

    /** Get the count for the color with the given RGB components */
    unsigned int getColor(unsigned char r, unsigned char g, unsigned char b) const;

    /** Increment the count for the given color */
    bool addColor(Color c);

private :
    // Map to store number of pixel of each color.
    tab2d *rgbSpace;
};

#endif
