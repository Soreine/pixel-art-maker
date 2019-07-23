#include <list>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

#include <CImg.h>
using namespace cimg_library;

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

#include "ColorHist.h"
#include "Color.h"
#include "HSVColor.h"
#include "Triplet.h"


void usage(const char * progname)
{
    cerr
            << "Usage: " << endl
            << "\t" << progname << " imageFile.png paletteFile.png patternFile.png [outputFile.png]" << endl
            << endl
            << "This program loads a palette image and an image and then try to" << endl
            << "display this image using the computed palette and a given" << endl
            << "threshold matrix." << endl
            << endl
            << "Parameters:" << endl
            << "\timageFile.png: path to the image file to dither" << endl
            << "\tpaletteFile.png: path to the palette file." << endl
            << "\tpatternFile.png: path to the image to use as a dithering pattern" << endl
            << "\toutputFile.png: result image will be saved under this name" << endl
            << "\t            (default to <imageFile>-dithered-by-<patternFile>-with-<paletteFile>.png)" << endl
            << endl
            << "Result:" << endl
            << "\tA dithered image will be saved." << endl;
    exit(1);
}

// Return the p-th value of the 8x8 Bayer threshold matrix
double bayer8x8(int p)
{
    int q = p ^ (p >> 3);
    long result = (((p & 4) >> 2) | ((q & 4) >> 1)
                   | ((p & 2) << 1) | ((q & 2) << 2)
                   | ((p & 1) << 4) | ((q & 1) << 5));
    return (double) result / 64;
}

inline Color getColor(int const x, int const y, CImg<unsigned char> const& image)
{
    return Color(image(x, y, 0), image(x, y, 1), image(x, y, 2));
}

/** Comparison function between two HSVColor, used to order
    colors. Sort by value, then saturation, then hue */
int HSVcompare (HSVColor const& a, HSVColor const& b)
{
    // Sort by value
    if( a.hsv.v > b.hsv.v )
    {
        return 1;
    }
    else if (a.hsv.v < b.hsv.v)
    {
        return -1;
    }
    else
    {
        // Sort by saturation
        if( a.hsv.s > b.hsv.s )
        {
            return 1;
        }
        else if ( a.hsv.s > b.hsv.s)
        {
            return -1;
        }
        else
        {
            // Sort by hue
            if( a.hsv.h > b.hsv.h )
            {
                return 1;
            }
            else if ( a.hsv.h > b.hsv.h)
            {
                return -1;
            }
            else
            {
                return 0;
            }
        }
    }
}

/** Parse a palette image and fill the palette Color array with all
    the colors. Assign the number of colors to the colorCount
    parameter. */
void parsePalette(CImg<unsigned char> const& paletteImage,
                  Color*& palette, int& colorCount)
{
    // A color histogram to check that we take each unique color only
    // once
    ColorHist ch;

    Color c;

    // Initialize the number of colors
    colorCount = 0;

    // Init the color array
    palette = new Color[paletteImage.height()*paletteImage.width()];
    cout << paletteImage.height()*paletteImage.width() << endl;
    // Parse the palette image and grab all the colors !
    for(int y = 0; y < paletteImage.height(); y++)
    {
        for(int x = 0; x < paletteImage.width(); x++)
        {
            // If not transparent
            if(paletteImage(x, y, 3) > 0)
            {
                c = getColor(x, y, paletteImage);
                // Add the color to the histogram and check it was
                // absent before
                if (!ch.addColor(c))
                {
                    // That's one more color :)
                    palette[colorCount] = c;
                    colorCount++;
                }
            }
        }
    }

    cout << "Color count : " << colorCount << endl;
    // Done
}

/** Parse a threshold image and init the threshold array with the
    threshold values.
    Assign the height and width of the threshold map to the passed
    variable.
    Assign the minimum and maximum threshold value of the map to
    the min and max variables */
void parseThreshold(CImg<unsigned char> const& thresholdImage, double*& threshold,
                    int& width, int& height, double& min, double& max)
{
    // A threshold value
    double value;

    // Set the width and height
    width = thresholdImage.width();
    height = thresholdImage.height();

    // Init the threshold array
    threshold = new double[width*height];

    // Parse the threshold image
    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            // Sum the threshold values
            value = (double) (thresholdImage(x,y,0) + thresholdImage(x,y,1) + thresholdImage(x,y,2));
            // Average and constrain to [0,1]
            value = value/(3*255);
            // Put in the map
            threshold[x + width*y] = value;
            // Update the min/max
            if(value < min)
                min = value;
            if(value > max)
                max = value;
        }
    }

    // Initialize the min/max with the first value
    min = threshold[0];
    max = min;
    // Find the max and min
    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            value = threshold[x + width*y];
            // Update the min/max
            if(value < min)
                min = value;
            if(value > max)
                max = value;
        }
    }

    cout << "Threshold Min " << min << " Max " << max << endl;
    // Done
}

/** Return an non dithered image, made with the input palette */
CImg<unsigned char>  nodither(CImg<unsigned char> const& image,
                              CImg<unsigned char> const& paletteImage)
{
    // The resulting image, reconstructed from the original with the palette's colors
    CImg<unsigned char> reconstructed(image.width(), image.height(), 1, 3, 0);

    // Temporary color variable
    Color c;
    // The nearest color index
    int nearest;
    // Distance of the nearest color
    long nearestDist;
    // Distance between two colors
    long dist;

    // The palette colors array
    Color * palette;
    // The number of colors in the palette
    int colorCount;

    // Grab the colors from the palette and set the colorCount
    parsePalette(paletteImage, palette, colorCount);

    // For each pixel in the image, find the best color
    for (int y = 0 ; y < image.height() ; y++)
    {
        for (int x = 0 ; x < image.width() ; x++)
        {
            // The color to approximate
            c = getColor(x, y, image);
            // The current nearest color is the first...
            nearest = 0;
            nearestDist = 16777216; // (256*256*256) = maximum value
            // of distance2(color1, color2)
            // Find the closest color in the palette
            for (int k = 0 ; k < colorCount ; k++ )
            {
                dist = Color::distance2(c, palette[k]);
                if (dist < nearestDist)
                {
                    nearestDist = dist;
                    nearest = k;
                }
            }
            // Set the target pixel with this color
            reconstructed(x, y, 0) = palette[nearest].getR();
            reconstructed(x, y, 1) = palette[nearest].getG();
            reconstructed(x, y, 2) = palette[nearest].getB();
        }
    }

    delete[] palette;

    // Return the result
    return reconstructed;
}


/** Return a dithered image, from the paletteImage colors and using
    the thresholdImage threshold. When mixing colors, takes the two nearest. */
CImg<unsigned char> ditherNearest(CImg<unsigned char> const& image,
                                  CImg<unsigned char> const& paletteImage,
                                  CImg<unsigned char> const& thresholdImage)
{
    // The resulting image, reconstructed from the original with the palette's colors
    CImg<unsigned char> dithered(image.width(), image.height(), 1, 3, 0);

    // Temporary color variable
    Color c;
    // The first nearest color index
    int nearest1;
    // The second nearest color index
    int nearest2;
    // Distance of the first nearest color
    long nearestDist1;
    // Distance of the second nearest color
    long nearestDist2;
    // Distance between two colors
    long dist;
    // The palette colors array
    Color * palette;
    // The palette colors array as HSVColors
    HSVColor * paletteHSV;
    // The number of colors in the palette
    int colorCount;

    // The threshold map (values between 0 and 1)
    double * threshold;
    // The width and height of the threshold map
    int tWidth;
    int tHeight;
    // The min and max values in the threshold map
    double tMin;
    double tMax;

    // Parse the threshold map
    parseThreshold(thresholdImage, threshold, tWidth, tHeight, tMin, tMax);

    // Grab the colors from the palette and set the colorCount
    parsePalette(paletteImage, palette, colorCount);

    // Initialize the paletteHSV
    paletteHSV = new HSVColor[colorCount];
    // Fill it
    for(int k = 0; k < colorCount; k++)
    {
        paletteHSV[k] = HSVColor(palette[k]);
    }
    // For each pixel in the image, find the best two colors
    for (int y = 0 ; y < image.height() ; y++)
    {
        for (int x = 0 ; x < image.width() ; x++)
        {
            // The color to approximate
            c = getColor(x, y, image);
            // The current nearest color is the first...
            nearest1 = 0;
            nearest2 = 0;
            nearestDist1 = 16777216; // (256*256*256) = maximum value
            // of distance2(color1, color2)
            nearestDist2 = nearestDist2;
            // Find the two closest color in the palette
            for (int k = 0 ; k < colorCount ; k++ )
            {
                // Calculate distance to this color
                dist = Color::distance2(c, palette[k]);
                // If this is the first best color
                if (dist < nearestDist1)
                {
                    // The old first color becomes the new second
                    nearestDist2 = nearestDist1;
                    nearest2 = nearest1;
                    // This color becomes the first nearest color
                    nearestDist1 = dist;
                    nearest1 = k;
                }
                else
                {
                    // Maybe it is better than the second color
                    if(dist < nearestDist2)
                    {
                        // This becomes the second nearest color
                        nearestDist2 = dist;
                        nearest2 = k;
                    }
                }
            }
            // nearest1 is the first nearest color, nearest2 is the second one

            // The corresponding threshold value
            double thresholdValue = threshold[(x % tWidth) + tWidth*(y % tHeight)];
            // Reorder the two colors if needed (for continuity when
            // going closer from the second one)
            if(HSVcompare(paletteHSV[nearest1], paletteHSV[nearest2]) > 0)
            {
                // Swap the two
                nearest1 = nearest1 ^ nearest2;
                nearest2 = nearest1 ^ nearest2;
                nearest1 = nearest1 ^ nearest2;

                nearestDist1 = nearestDist1 ^ nearestDist2;
                nearestDist2 = nearestDist1 ^ nearestDist2;
                nearestDist1 = nearestDist1 ^ nearestDist2;
            }
            // Our color is located between two colors :
            double value = (double) nearestDist1 / (double) (nearestDist1 + nearestDist2);
            // Pick one of the two best color, regarding to the threshold map
            int chosen;
            if(value < thresholdValue)
                chosen = nearest1;
            else
                chosen = nearest2;

            // Set the target pixel with this color
            dithered(x, y, 0) = palette[chosen].getR();
            dithered(x, y, 1) = palette[chosen].getG();
            dithered(x, y, 2) = palette[chosen].getB();
        }
    }

    // Return the result
    return dithered;
}

int main(int argc, char* argv[])
{

    // The image file path
    char * fileImage;
    // The palette image file path
    char * filePalette;
    // The threshold pattern image file path
    char * fileThreshold;

    // If not enough arguments were given when called
    if (argc != 4 && argc != 5)
    {
        // Display an error and exit
        usage(argv[0]);
    }

    fileImage = argv[1];
    filePalette = argv[2];
    fileThreshold = argv[3];

    // The image
    CImg<unsigned char>  image(fileImage);
    // The palette image
    CImg<unsigned char>  paletteImage(filePalette);
    // The threshold image
    CImg<unsigned char>  thresholdImage(fileThreshold);

    // The dithered result image
    CImg<unsigned char>  result(fileThreshold);

    result = ditherNearest(image, paletteImage, thresholdImage);

    /* Display the result
    CImgDisplay result_disp(result, "Result");
    cout << "Enter to finish" << endl;
    cin.ignore();
    */

    // Save dithered image
    string outputFilename;
    if(argc >= 5)
    {
        outputFilename = string(argv[4]);
    }
    else
    {
        // Create a generic file name
        fs::path pathname(fileImage);
        string dirname  = pathname.parent_path().string();
        string basename = pathname.stem().string();

        fs::path palettepathname(filePalette);
        string palettebasename = palettepathname.stem().string();

        fs::path patternpathname(fileThreshold);
        string patternbasename = patternpathname.stem().string();
        if (dirname.length() == 0)
        {
            outputFilename = basename + "-dithered-by-" + palettebasename + "-with-" + patternbasename + ".png";
        }
        else
        {
            outputFilename = dirname + "/" + basename + "-dithered-by-" + palettebasename + "-with-" + patternbasename + ".png";
        }
    }

    result.save(outputFilename.c_str());
    cout << "Saved to " << outputFilename << endl;

    return 0;
}
