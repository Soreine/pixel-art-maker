#include <list>
#include <iostream>
#include <sstream>
#include <string>
#include <stdlib.h>
using namespace std;

#include <CImg.h>
using namespace cimg_library;

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

#include "ColorHist.h"
#include "Color.h"
#include "HSVColor.h"
#include "Triplet.h"

void usage(const char * progname) {
  cerr
    << "Usage: " << endl
    << "\t" << progname << " imageFile colorsCount [outputFile]" << endl
    << endl
    << "This program determines the best color palette to use to display an" << endl
    << "image, using the K-mean algorithm." << endl
    << endl
    << "Parameters:" << endl
    << "\timageFile: path to the image file." << endl
    << "\tcolorsCount: desired number of colors in the palette used" << endl
    << "\toutputFile: name for the output palette image file" << endl
    << "\t(default outputFile 'palette-colorCount-<imageFile>.png')" << endl
    << endl
    << "Result:" << endl
    << "\tAn image containing the palette's colors will be saved." << endl;
  exit(1);
}


/** This struct represent a point in the RGB space, along with the
    cluster it currently belongs to. */
struct point {
    // The RGB coordinates
    Color color;
    // The cluster index this point belongs to
    unsigned int cluster;
};


inline Color getPixel(int const x, int const y, CImg<unsigned char> const& image) {
    return Color(image(x, y, 0), image(x, y, 1), image(x, y, 2));
}

/** Comparison function between two HSVColor, used for quicksort */
int HSVcompare (const void * a, const void * b)
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

/** Generate an return an image containing the optimal K-colors
    palette for the input image. */
CImg<unsigned char> generatePalette(CImg<unsigned char> const image, int const K) {

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

    // The centers of the clusters in the k-mean algorithm. At the end:
    // the computed color palette
    Color * kmean = new Color[K];
    // The next computed kmeans. Acts as an intermediate buffer 
    Triplet * nextKMean = new Triplet[K];

    // Initialize the color histogram and the point list

    for (int y = 0 ; y < image.height() ; y++) {
	for (int x = 0 ; x < image.width() ; x++) { // For each pixel
	    // 
	    p.color = getPixel(x, y, image);
	    p.cluster = 0; 
	    // Add the color to the histogram
	    if (!ch.addColor(p.color))
		// If the color didn't exist already, add it to the list of all the differents colors
		pointList.push_front(p);
	}
    }

    // Check that there are enough different colors in the original image
    if (pointList.size() < (unsigned int) K) { 
	// Not enough colors
	cerr << "The original image has less colors than the desired number of colors in the palette." << endl;
	exit(1);
    }

    // Initialize the clusters centers (kmeans) with colors from the image
    {
	// Select K colors evenly spaced in the list of all unique colors
	list<point>::iterator iterator = pointList.begin();
	unsigned int step = pointList.size()/(unsigned int) K;
	for (int i = 0; i < K; i++) {
	    // Initialize the kmean with this color
	    c = (*iterator).color;
	    kmean[i] = c;
	    // Next step
	    std::advance(iterator, step);
	}
    }
  
    ////////////////// K-Mean Algorithm ////////////////////////


    // While the algorithm modifies the kmeans
    while(changed) {
	// Initialize the next k-means array
	for (int i = 0 ; i < K ; i++ ) {
	    nextKMean[i] = Triplet(0,0,0);
	    clusterWeight[i] = 0;
	}
     
	// Update the clusters by re-associating the points to the closest kmean
	for (list<point>::iterator iterator = pointList.begin() ; 
	     iterator != pointList.end() ; iterator++) {
	    // Get the color being treated
	    c = (*iterator).color;
       
	    // Initialize the nearest kmean as the first of the list
	    nearest = 0;
	    // Initialize with the maximum distance
	    nearestDist = 4096; // sqrt(256*256*256)
	    // Find the closest kmean
	    for (int i = 0 ; i < K ; i++ ) {
		// Distance from the i-th kmean
		dist = Color::distance2(c, kmean[i]);
		// If the distance is best
		if (dist < nearestDist) {
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
	for (int i = 0 ; i < K ; i++ ) {
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
    int height = K/256 + 1;
    int width = K<255?K:256; // minimum between K and 256

    // Create the image that will contain the palette colors, the
    // fourth channel is for transparency
    CImg<unsigned char> palette(width, height, 1, 4, 0);

    // Sort the palette color
    {
	// Create an HSV palette
	HSVColor * hsvPalette = new HSVColor[K];
	// Fill it with the RGB colors from the palette
	for(int i = 0; i < K; i++) {
	    hsvPalette[i] = HSVColor(kmean[i]);
	}
	// Quicksort the palette
	qsort(hsvPalette, K, sizeof(HSVColor), HSVcompare);

	// Replace the RGB palette with the ordered values
	for(int i = 0; i < K; i++) {
	    kmean[i] = hsvPalette[i].toRGBColor();
	}
	
	delete[] hsvPalette;
    }

    // Generate the palette image
    for (int n = 0 ; n < K ; n++ ) {
	// Get the palette color
	c = kmean[n];
	// Create a pixel of the color c in the palette image
	// Line
	int y = n/palette.width();
	// Column
	int x = n%palette.width();
	// Fill the RGB channels
	palette(x, y, 0) = c.getR();
	palette(x, y, 1) = c.getG();
	palette(x, y, 2) = c.getB();
	palette(x, y, 3) = 255; // Opacity 100%
    }

    // Free memory
    delete[] nextKMean;
    delete[] kmean;
    delete[] clusterWeight;
    return palette;
}


int main(int argc, char* argv[]) {

    // The image file path
    char * file;
    // The output destination file path
    string outputFile;
    // The corresponding image
    CImg<unsigned char> image;
    // The generated palette image
    CImg<unsigned char> palette;
    // The number of colors in the palette. This correspond to the K of
    // the K-mean algorithm.
    int K;

    // If not enough arguments were given when called
    if (argc < 3) {
	// Display an error and exit
      usage(argv[0]);
    }
  
    // Retrieve the file path argument
    file = argv[1];
    // Retrieve the number of colors argument
    K = atoi(argv[2]);
    // If was given the output file name
    if(argc == 4) {
      outputFile = string(argv[3]);
    } else {
      // or create a generic file name
      stringstream ss;
      ss << K;
      fs::path pathname(file);
      string dirname  = pathname.parent_path().string();
      string basename = pathname.stem().string();

      outputFile =
	dirname + "/"
	+ "palette-" + ss.str() + "-"
	+ basename + ".png";
    }
    // Check that K is between 2 and 65536
    if(K < 2 || K > 65536) {
	// Error and exit
      cerr << "*** Color Count must be between 2 and 65536!" << endl << endl;
	usage(argv[0]);
    }

    // Create the image
    image = CImg<unsigned char>(file);
    
    palette = generatePalette(image, K);


    // Save the palette image
    palette.save(outputFile.c_str());
    cout << "Saved to " << outputFile << endl;

    return 0;
}
