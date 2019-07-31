#include "pixam.h"

/** This struct represent a point in the RGB space, along with the
    cluster it currently belongs to. */
struct point
{
    // The RGB coordinates
    Color color;
    // The cluster index this point belongs to
    unsigned int cluster;
};

// Return the p-th value of the 8x8 Bayer threshold matrix
double bayer8x8(int p)
{
    int q = p ^ (p >> 3);
    long result = (((p & 4) >> 2) | ((q & 4) >> 1)
                   | ((p & 2) << 1) | ((q & 2) << 2)
                   | ((p & 1) << 4) | ((q & 1) << 5));
    return (double) result / 64.0;
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

/** Comparison function between two HSVColor, used for quicksort */
int HSVcompareL (const void * a, const void * b)
{
    // Sort by hue
    double aValue = ((HSVColor*)a)->hsv.h;
    double bValue = ((HSVColor*)b)->hsv.h;
    if( aValue > bValue )
        return 1;
    else if (aValue < bValue)
        return -1;
    else
        return 0;
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
    // The index in loops
    int y, x;

    // Initialize the number of colors
    colorCount = 0;

    // Init the color array
    palette = new Color[paletteImage.height()*paletteImage.width()];
    cout << "Palette count : " << paletteImage.height()*paletteImage.width() << endl;
    // Parse the palette image and grab all the colors !
    for(y = 0; y < paletteImage.height(); y++)
    {
        for(x = 0; x < paletteImage.width(); x++)
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
    // Set the width and height
    width = thresholdImage.width();
    height = thresholdImage.height();
    // The index in loops
    int y, x, k;
    // A threshold value
    double value;

    // Init the threshold array
    threshold = new double[width*height];

    // Parse the threshold image
    k = 0;
    for(y = 0; y < height; y++)
    {
        for(x = 0; x < width; x++)
        {
            // Sum the threshold values
            value = (double) (thresholdImage(x,y,0) + thresholdImage(x,y,1) + thresholdImage(x,y,2));
            // Average and constrain to [0,1]
            value = value/(3*255);
            // Put in the map
            threshold[k] = value;
            k++;
        }
    }

    // Initialize the min/max with the first value
    min = threshold[0];
    max = min;
    // Find the max and min
    k = 0;
    for(y = 0; y < height; y++)
    {
        for(x = 0; x < width; x++)
        {
            value = threshold[k];
            // Update the min/max
            min = (value < min) ? value : min;
            max = (value > max) ? value : max;
            k++;
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
    // The index in loops
    int y, x, k;

    // Grab the colors from the palette and set the colorCount
    parsePalette(paletteImage, palette, colorCount);

    // For each pixel in the image, find the best color
    for (y = 0; y < image.height(); y++)
    {
        for (x = 0; x < image.width(); x++)
        {
            // The color to approximate
            c = getColor(x, y, image);
            // The current nearest color is the first...
            nearest = 0;
            nearestDist = 16777216; // (256*256*256) = maximum value
            // of distance2(color1, color2)
            // Find the closest color in the palette
            for (k = 0; k < colorCount; k++)
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
    // The index in loops
    int y, x, k;

    // The threshold map (values between 0 and 1)
    double * threshold;
    // The width and height of the threshold map
    int tWidth, tHeight;
    // The min and max values in the threshold map
    double tMin, tMax;
    // The corresponding threshold value
    double thresholdValue, value;
    // The index picking one of the two best color
    int chosen;

    // Parse the threshold map
    parseThreshold(thresholdImage, threshold, tWidth, tHeight, tMin, tMax);

    // Grab the colors from the palette and set the colorCount
    parsePalette(paletteImage, palette, colorCount);

    // Initialize the paletteHSV
    paletteHSV = new HSVColor[colorCount];
    // Fill it
    for(k = 0; k < colorCount; k++)
    {
        paletteHSV[k] = HSVColor(palette[k]);
    }
    // For each pixel in the image, find the best two colors
    for (y = 0; y < image.height(); y++)
    {
        for (x = 0; x < image.width(); x++)
        {
            // The color to approximate
            c = getColor(x, y, image);
            // The current nearest color is the first...
            nearest1 = 0;
            nearest2 = 0;
            nearestDist1 = 16777216; // (256*256*256) = maximum value
            // of distance2(color1, color2)
            nearestDist2 = nearestDist1;
            // Find the two closest color in the palette
            for (k = 0; k < colorCount; k++ )
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
            thresholdValue = threshold[(x % tWidth) + tWidth*(y % tHeight)];
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
            value = (double) nearestDist1 / (double) (nearestDist1 + nearestDist2);
            // Pick one of the two best color, regarding to the threshold map
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

/** Generate an return an image containing the optimal K-colors
    palette for the input image. */
CImg<unsigned char> generatePalette(CImg<unsigned char> const image, int const K)
{

    // Create a display for the original image
    // CImgDisplay orig_disp(image, "Original");

    // The list of the uniques colors from the image. It corresponds to
    // the points (in RGB space) for the K-mean algorithm.
    list<point> pointList;
    // The number of occurrence of each color from the RGB space in the
    // input image.
    ColorHist ch;
    // Temporary variable
    Color c;
    // Temporary variable
    point p;


    // Indicates if the kmeans have changed during the last step. False
    // means that the algorithm has converged.
    bool changed = true;
    // Index of the nearest color
    int nearest;
    // The distance from the nearest color
    double nearestDist;
    // The temporary calculated distance
    double dist;
    // The number of point associated with each cluster
    long * clusterWeight = new long[K];
    // Temporary weight
    long weight;
    // The index in loops
    int y, x, i;
    // The list of all unique colors
    list<point>::iterator iterator;
    unsigned int step;
    // The palette image size
    int height, width;

    // The centers of the clusters in the k-mean algorithm. At the end:
    // the computed color palette
    Color * kmean = new Color[K];
    // The next computed kmeans. Acts as an intermediate buffer
    Triplet * nextKMean = new Triplet[K];

    // Initialize the color histogram and the point list

    for (y = 0; y < image.height(); y++)
    {
        for (x = 0; x < image.width(); x++)   // For each pixel
        {
            //
            p.color = getColor(x, y, image);
            p.cluster = 0;
            // Add the color to the histogram
            if (!ch.addColor(p.color))
                // If the color didn't exist already, add it to the list of all the differents colors
                pointList.push_front(p);
        }
    }

    // Check that there are enough different colors in the original image
    if (pointList.size() < (unsigned int) K)
    {
        // Not enough colors
        cerr << "The original image has less colors than the desired number of colors in the palette." << endl;
        exit(1);
    }

    // Initialize the clusters centers (kmeans) with colors from the image
    // Select K colors evenly spaced in the list of all unique colors
    iterator = pointList.begin();
    step = pointList.size()/(unsigned int) K;
    for (i = 0; i < K; i++)
    {
        // Initialize the kmean with this color
        c = (*iterator).color;
        kmean[i] = c;
        // Next step
        std::advance(iterator, step);
    }

    ////////////////// K-Mean Algorithm ////////////////////////

    // While the algorithm modifies the kmeans
    while(changed)
    {
        // Initialize the next k-means array
        for (i = 0; i < K; i++)
        {
            nextKMean[i] = Triplet(0,0,0);
            clusterWeight[i] = 0;
        }

        // Update the clusters by re-associating the points to the closest kmean
        for (iterator = pointList.begin(); iterator != pointList.end(); iterator++)
        {
            // Get the color being treated
            c = (*iterator).color;

            // Initialize the nearest kmean as the first of the list
            nearest = 0;
            // Initialize with the maximum distance
            nearestDist = 16777216; // (256*256*256) = maximum value
            // Find the closest kmean
            for (i = 0; i < K; i++)
            {
                // Distance from the i-th kmean
                dist = Color::distance2(c, kmean[i]);
                // If the distance is best
                if (dist < nearestDist)
                {
                    // Update nearest variables
                    nearestDist = dist;
                    nearest = i;
                }
            }
            // This color belongs to the cluster of the nearest kmean
            (*iterator).cluster = nearest;

            // Account the number of pixel of this color
            weight = ch.getColor(c);
            // Convert to a triplet
            Triplet triplet = c.toTriplet();
            // Multiply by the weight
            triplet.multiply(weight);
            // Add it to the total
            nextKMean[nearest].add(triplet);

            // Add to the total weight of the nearest kmean
            clusterWeight[nearest] += weight;

        }
        // All color pixels have been re-assigned to their nearest cluster


        // Divide the sums of color of each kmean by their weight. The
        // calculated barycenter is the new kmean. Finally we check
        // whether this has changed the kmeans.

        changed = false;
        // For each kmean
        for (i = 0; i < K; i++)
        {
            // If the cluster is not empty
            if(clusterWeight[i] != 0)
                // Calculate the barycenter
                nextKMean[i].divide(clusterWeight[i]);
            // Convert as color
            c = nextKMean[i].getColor();
            // Update the changed boolean
            changed = changed or
                      (c.getR() != kmean[i].getR()) or
                      (c.getG() != kmean[i].getG()) or
                      (c.getB() != kmean[i].getB());
            // Update the kmean
            kmean[i] = c;
        }
        cout << "+";
        cout.flush();
    }
    // The algorithm has converged. The kmean represent our colors
    cout << endl;

    // Determine the palette image size (at most 256 pixels wide)
    height = K / 256 + 1;
    width = (K < 255) ? K : 256; // minimum between K and 256

    // Create the image that will contain the palette colors, the
    // fourth channel is for transparency
    CImg<unsigned char> palette(width, height, 1, 4, 0);

    // Sort the palette color
    {
        // Create an HSV palette
        HSVColor * hsvPalette = new HSVColor[K];
        // Fill it with the RGB colors from the palette
        for(i = 0; i < K; i++)
        {
            hsvPalette[i] = HSVColor(kmean[i]);
        }
        // Quicksort the palette
        qsort(hsvPalette, K, sizeof(HSVColor), HSVcompareL);

        // Replace the RGB palette with the ordered values
        for(i = 0; i < K; i++)
        {
            kmean[i] = hsvPalette[i].toRGBColor();
        }

        delete[] hsvPalette;
    }

    // Generate the palette image
    for (i = 0; i < K; i++)
    {
        // Get the palette color
        c = kmean[i];
        // Create a pixel of the color c in the palette image
        // Line
        y = i/palette.width();
        // Column
        x = i%palette.width();
        // Fill the RGB channels
        palette(x, y, 0) = c.getR();
        palette(x, y, 1) = c.getG();
        palette(x, y, 2) = c.getB();
        palette(x, y, 3) = 255; // Opacity 100%
    }
    cout << "Palette count : " << K << endl;

    // Free memory
    delete[] nextKMean;
    delete[] kmean;
    delete[] clusterWeight;
    return palette;
}

/** Generate an return an image containing the pattern. */
CImg<unsigned char> generatePattern(CImg<unsigned char> const image, int const K)
{
    // The resulting image, reconstructed from the original with the pattern
    CImg<unsigned char> pattern(K, K, 1, 3, 0);

    // Temporary color variable
    Color c;
    // The index in loops
    int y, x, i, j, yi, xj, ii, ij;

    // The threshold map (values between 0 and 1)
    double * threshold;
    // The width and height of the threshold map
    int count = 0, Karea = K * K;
    // The min and max values in the threshold map
    double tMin, tMax, kM;
    // Temporary value
    double value = 0.0;
    // Selective colors
    int ivalue;

    // Init the threshold array
    threshold = new double[K*K];
    for (i = 0; i < Karea; i++)
    {
        threshold[i] = 0.0;
    }
    // For each pixel in the image: sum
    for (y = 0; y < image.height(); y+=K)
    {
        for (x = 0; x < image.width(); x+=K)
        {
            ii = 0;
            for (i = 0; i < K; i++)
            {
                yi = y + i;
                if (yi >= image.height())
                {
                    yi = image.height() + image.height() - yi - 2;
                }
                for (j = 0; j < K; j++)
                {
                    xj = x + j;
                    if (xj >= image.width())
                    {
                        xj = image.width() + image.width() - xj - 2;
                    }
                    ij = ii + j;
                    c = getColor(xj, yi, image);
                    value = (double) (c.getR() + c.getG() + c.getB());
                    value /= 765.0;
                    threshold[ij] += value;
                }
                ii += K;
            }
            count++;
        }
    }
    // For each pixel in the pattern: mean
    tMin = 1.0;
    tMax = 0.0;
    for (i = 0; i < Karea; i++)
    {
        value = threshold[i];
        value /= (double)count;
        value = 1.0 - value;
        threshold[i] = value;
        tMin = (value < tMin) ? value : tMin;
        tMax = (value > tMax) ? value : tMax;
    }
    if ((tMax - tMin) > 0)
    {
        kM = 1.0 / (tMax - tMin);
    } else {
        kM = 1.0;
        tMin -= 0.5;
        tMax = tMin;
    }
    // For each pixel in the pattern: grayscale
    ii = 0;
    for (i = 0; i < K; i++)
    {
        for (j = 0; j < K; j++)
        {
            ij = ii + j;
            value = threshold[ij];
            value -= tMin;
            value *= kM;
            ivalue = (int) (255.0 * value + 0.5);
            ivalue = (ivalue < 0) ? 0 : ivalue;
            ivalue = (ivalue > 255) ? 255 : ivalue;
            pattern(j, i, 0) = ivalue;
            pattern(j, i, 1) = ivalue;
            pattern(j, i, 2) = ivalue;
        }
        ii += K;
    }
    cout << "Threshold Min " << tMin << " kM " << kM << endl;
    // Return the result
    return pattern;
}

CImg<unsigned char> undither(CImg<unsigned char> const& image,
                             CImg<unsigned char> const& thresholdImage)
{
    // The resulting image, reconstructed from the original with the palette's colors
    CImg<unsigned char> undithered(image.width(), image.height(), 1, 3, 0);

    // Temporary color variable
    Color c;
    // The index in loops
    int y, x, i, j, ii, ij;

    // The threshold map (values between 0 and 1)
    double * threshold;
    // The width and height of the threshold map
    int tWidth, tHeight, tArea, count, n = 0;
    // The min and max values in the threshold map
    double tMin, tMax;
    // sums OLS
    double sxy, sx, sy, sxx, t, a, b, ab;
    // Temporary value
    double value = 0.0;
    // Statistics
    double sv = 0.0, sa = 0.0, sb = 0.0;
    // Selective colors
    int ivalue;
    double scR, scG, scB, scRGB, kR, kG, kB;

    // Parse the threshold map
    parseThreshold(thresholdImage, threshold, tWidth, tHeight, tMin, tMax);
    tArea = tWidth * tHeight;

    // For each pixel in the image, linear OLS pattern
    for (y = 0; y < image.height(); y+=tHeight)
    {
        for (x = 0; x < image.width(); x+=tWidth)
        {
            // linear OLS: c = a * pattern + b
            sxy = 0.0;
            sxx = 0.0;
            sx = 0.0;
            sy = 0.0;
            count = 0;
            ii = 0;
            for (i = 0; i < tHeight; i++)
            {
                if ((y + i) < image.height())
                {
                    for (j = 0; j < tWidth; j++)
                    {
                        if ((x + j) < image.width())
                        {
                            ij = ii + j;
                            c = getColor(x + j, y + i, image);
                            value = (double) (c.getR() + c.getG() + c.getB());
                            value /= 765.0;
                            t = threshold[ij];
                            sx += t;
                            sxx += (t * t);
                            sy += value;
                            sxy += (value * t);
                            count++;
                        }
                    }
                }
                ii += tWidth;
            }
            ab = (double)count * sxx - sx * sx;
            a = 0.0;
            b = 0.0;
            if (ab == 0)
            {
                a = 0.0;
                b = (sy - sx) / (double)count;
            } else {
                a = ((double)count * sxy - sx * sy) / ab;
                b = (sy - a * sx) / (double)count;
            }
            // Mean colors
            sa += a;
            sb += b;
            n++;
            sv = 0;
            scR = 0;
            scG = 0;
            scB = 0;
            ii = 0;
            for (i = 0; i < tHeight; i++)
            {
                if ((y + i) < image.height())
                {
                    for (j = 0; j < tWidth; j++)
                    {
                        if ((x + j) < image.width())
                        {
                            ij = ii + j;
                            c = getColor(x + j, y + i, image);
                            value = (double) (c.getR() + c.getG() + c.getB());
                            value /= 765.0;
                            t = threshold[ij];
                            t *= a;
                            t += b;
                            value -= t;
                            value *= value;
                            value = 1.0 - value;
                            sv += value;
                            scR += (double) c.getR() * value;
                            scG += (double) c.getG() * value;
                            scB += (double) c.getB() * value;
                        }
                    }
                }
                ii += tWidth;
            }
            sv = (sv == 0.0) ? (double)tArea : sv;
            scR /= sv;
            scG /= sv;
            scB /= sv;
            scRGB = scR + scG + scB;
            scRGB = (scRGB > 0) ? scRGB : 383.0;
            scRGB = 765.0 / scRGB;
            kR = scR * scRGB;
            kB = scB * scRGB;
            kG = scG * scRGB;
            // Unpattern
            ii = 0;
            for (i = 0; i < tHeight; i++)
            {
                if ((y + i) < image.height())
                {
                    for (j = 0; j < tWidth; j++)
                    {
                        if ((x + j) < image.width())
                        {
                            ij = ii + j;
                            c = getColor(x + j, y + i, image);
                            t = threshold[ij];
                            t *= a;
                            t += b;
                            value = (double) c.getR() - t * kR + scR;
                            ivalue = (int) (value + 0.5);
                            ivalue = (ivalue < 0) ? 0 : ivalue;
                            ivalue = (ivalue > 255) ? 255 : ivalue;
                            undithered(x + j, y + i, 0) = ivalue;
                            value = (double) c.getG() - t * kG + scG;
                            ivalue = (int) (value + 0.5);
                            ivalue = (ivalue < 0) ? 0 : ivalue;
                            ivalue = (ivalue > 255) ? 255 : ivalue;
                            undithered(x + j, y + i, 1) = ivalue;
                            value = (double) c.getB() - t * kB + scB;
                            ivalue = (int) (value + 0.5);
                            ivalue = (ivalue < 0) ? 0 : ivalue;
                            ivalue = (ivalue > 255) ? 255 : ivalue;
                            undithered(x + j, y + i, 2) = ivalue;
                        }
                    }
                }
                ii += tWidth;
            }
        }
    }
    cout << "Coeff : a: " << (sa / n) << " b: " << (sb / n) << endl;
    // Return the result
    return undithered;
}

CImg<unsigned char> unditherscan(CImg<unsigned char> const& image,
                                 CImg<unsigned char> const& thresholdImage)
{
    // The resulting image, reconstructed from the original with the palette's colors
    CImg<unsigned char> undithered(image.width(), image.height(), 1, 3, 0);

    // Limits
    int Yend, Xend;
    // Temporary color variable
    Color c;
    // The index in loops
    int y, x, i, j, ii, ij;

    // The threshold map (values between 0 and 1)
    double * threshold;
    // The width and height of the threshold map
    int tWidth, tHeight, tArea, count, n = 0;
    // The min and max values in the threshold map
    double tMin, tMax;
    // sums OLS
    double sxy, sx, sy, sxx, t, a, b, ab;
    // Temporary value
    double value = 0.0;
    // Statistics
    double sv = 0.0, sa = 0.0, sb = 0.0;
    // Selective colors
    int ivalue;
    double scR, scG, scB, scRGB, kR, kG, kB;

    // Parse the threshold map
    parseThreshold(thresholdImage, threshold, tWidth, tHeight, tMin, tMax);
    tArea = tWidth * tHeight;

    // Copy image
    for (y = 0; y < image.height(); y++)
    {
        for (x = 0; x < image.width(); x++)
        {
            for (i = 0; i < 3; i++)
            {
                undithered(x, y, i) = image(x, y, i);
            }
        }
    }
    // Limits
    Yend = undithered.height() - thresholdImage.height() + 1;
    Xend = undithered.width() - thresholdImage.width() + 1;
    Yend = (Yend > 0) ? Yend : 1;
    Xend = (Xend > 0) ? Xend : 1;
    // For each pixel in the image, linear OLS pattern
    for (y = 0; y < Yend; y++)
    {
        for (x = 0; x < Xend; x++)
        {
            // linear OLS: c = a * pattern + b
            sxy = 0.0;
            sxx = 0.0;
            sx = 0.0;
            sy = 0.0;
            count = 0;
            ii = 0;
            for (i = 0; i < tHeight; i++)
            {
                if ((y + i) < undithered.height())
                {
                    for (j = 0; j < tWidth; j++)
                    {
                        if ((x + j) < undithered.width())
                        {
                            ij = ii + j;
                            c = getColor(x + j, y + i, undithered);
                            value = (double) (c.getR() + c.getG() + c.getB());
                            value /= 765.0;
                            t = threshold[ij];
                            sx += t;
                            sxx += (t * t);
                            sy += value;
                            sxy += (value * t);
                            count++;
                        }
                    }
                }
                ii += tWidth;
            }
            ab = (double)count * sxx - sx * sx;
            a = 0.0;
            b = 0.0;
            if (ab == 0)
            {
                a = 0.0;
                b = (sy - sx) / (double)count;
            } else {
                a = ((double)count * sxy - sx * sy) / ab;
                b = (sy - a * sx) / (double)count;
            }
            // Mean colors
            sa += a;
            sb += b;
            n++;
            sv = 0;
            scR = 0;
            scG = 0;
            scB = 0;
            ii = 0;
            for (i = 0; i < tHeight; i++)
            {
                if ((y + i) < undithered.height())
                {
                    for (j = 0; j < tWidth; j++)
                    {
                        if ((x + j) < undithered.width())
                        {
                            ij = ii + j;
                            c = getColor(x + j, y + i, undithered);
                            value = (double) (c.getR() + c.getG() + c.getB());
                            value /= 765.0;
                            t = threshold[ij];
                            t *= a;
                            t += b;
                            value -= t;
                            value *= value;
                            value = 1.0 - value;
                            sv += value;
                            scR += (double) c.getR() * value;
                            scG += (double) c.getG() * value;
                            scB += (double) c.getB() * value;
                        }
                    }
                }
                ii += tWidth;
            }
            sv = (sv == 0.0) ? (double)tArea : sv;
            scR /= sv;
            scG /= sv;
            scB /= sv;
            scRGB = scR + scG + scB;
            scRGB = (scRGB > 0) ? scRGB : 383.0;
            scRGB = 765.0 / scRGB;
            kR = scR * scRGB;
            kB = scB * scRGB;
            kG = scG * scRGB;
            // Unpattern
            ii = 0;
            for (i = 0; i < tHeight; i++)
            {
                if ((y + i) < undithered.height())
                {
                    for (j = 0; j < tWidth; j++)
                    {
                        if ((x + j) < undithered.width())
                        {
                            ij = ii + j;
                            c = getColor(x + j, y + i, undithered);
                            t = threshold[ij];
                            t *= a;
                            t += b;
                            value = (double) c.getR() - t * kR + scR;
                            ivalue = (int) (value + 0.5);
                            ivalue = (ivalue < 0) ? 0 : ivalue;
                            ivalue = (ivalue > 255) ? 255 : ivalue;
                            undithered(x + j, y + i, 0) = ivalue;
                            value = (double) c.getG() - t * kG + scG;
                            ivalue = (int) (value + 0.5);
                            ivalue = (ivalue < 0) ? 0 : ivalue;
                            ivalue = (ivalue > 255) ? 255 : ivalue;
                            undithered(x + j, y + i, 1) = ivalue;
                            value = (double) c.getB() - t * kB + scB;
                            ivalue = (int) (value + 0.5);
                            ivalue = (ivalue < 0) ? 0 : ivalue;
                            ivalue = (ivalue > 255) ? 255 : ivalue;
                            undithered(x + j, y + i, 2) = ivalue;
                        }
                    }
                }
                ii += tWidth;
            }
        }
    }
    cout << "Coeff : a: " << (sa / n) << " b: " << (sb / n) << endl;
    // Return the result
    return undithered;
}
