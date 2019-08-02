#ifndef PIXAM_H
#define PIXAM_H

#include <list>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

#include <CImg.h>
using namespace cimg_library;

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

#include <math.h>
#include "ColorHist.h"
#include "Color.h"
#include "HSVColor.h"
#include "Triplet.h"

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
#define TRIM(x,a,b) (MIN(MAX(x,a),b))
#endif

CImg<unsigned char> generatePalette(CImg<unsigned char> const, int const);
CImg<unsigned char> generatePattern(CImg<unsigned char> const, int const);
CImg<unsigned char> generatePatternSymmetric(CImg<unsigned char> const, int const);
CImg<unsigned char> ditherNearest(CImg<unsigned char> const&,
                                  CImg<unsigned char> const&,
                                  CImg<unsigned char> const&);
CImg<unsigned char> undither(CImg<unsigned char> const&,
                             CImg<unsigned char> const&);
CImg<unsigned char> unditherscan(CImg<unsigned char> const&,
                                 CImg<unsigned char> const&);
#endif //PIXAM_H//
