#include <list>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

#include "CImg-1.5.7/CImg.h"
using namespace cimg_library;

#include "lib/ColorHist.h"
#include "lib/Color.h"
#include "lib/Triplet.h"


/** 
    This program load an image and a palette image then try to display
    this image using the computed palette and a given threshold
    matrix.
    Use : 'ditherer %filepath %colorsCount'
    Parameters :
    filepath : The path to the image file.
    colorsCount : The desired number of colors in the palette used
    Result :
    The reconstructed image and an image containing the palette's
    colors will be saved.
*/


inline Color getPixel(int const x, int const y, CImg<unsigned char> const& image) {
    return Color(image(x, y, 0), image(x, y, 1), image(x, y, 2));
}

void reconstruct_basic(CImg<unsigned char> image, Color * palette, int size) {
    /////////////// Reconstruction ///////////////////
    CImg<unsigned char> reconstructed(image.width(), image.height(), 1, 3, 0);
    CImgDisplay reconstructed_disp(reconstructed, "Image reconstruite");
    Color c;
    int nearest;
    double nearestDist, dist;

    for (int i = 0 ; i < image.width() ; i++) {
	for (int j = 0 ; j < image.height() ; j++) {
	    c = getPixel(i, j, image);
      
	    nearest = 0;
	    nearestDist = 4096; // sqrt(256*256*256)
	    // Calculer la moyenne la plus proche
	    for (int k = 0 ; k < size ; k++ ) {
		dist = Color::colorCompare(c, palette[k]);
		if (dist < nearestDist) {
		    nearestDist = dist;
		    nearest = k;
		}
	    }
	    reconstructed(i, j, 0) = palette[nearest].getR();
	    reconstructed(i, j, 1) = palette[nearest].getG();
	    reconstructed(i, j, 2) = palette[nearest].getB();
	}
    }


    reconstructed.display(reconstructed_disp);
    reconstructed.save("reconstructed-basic.png");

    cin.ignore();
}


struct mix{
    Color color;
    Color c1;
    Color c2;
};

void reconstruct_bayer(CImg<unsigned char> image, Color * palette, int size) {
  
    CImg<unsigned char> reconstructed(image.width(), image.height(), 1, 3, 0);
    CImgDisplay reconstructed_disp(reconstructed, "Image reconstruite");
    Color c;
    int nearest, k;
    double nearestDist, dist;
    bool isMixed;
    int nbMixed = size*(size-1)/2;
    mix * extendedPalette = new mix[nbMixed];
  
    // Compute the possible mix of color
    k = 0;
    for(int i = 0; i < size; i++) {
	for(int j = i+1; j < size; j++) {
	    extendedPalette[k].color = Color::mix(palette[i], palette[j], 0.5); // The half mix between i and j
	    extendedPalette[k].c1 = palette[i];
	    extendedPalette[k].c2 = palette[j];
	    k++;
	}
    }

    // Reconstruction
    for (int i = 0 ; i < image.width() ; i++) {
	for (int j = 0 ; j < image.height() ; j++) {
	    c = getPixel(i, j, image);

	    isMixed = false;
      
	    nearest = 0;
	    nearestDist = 4096; // sqrt(256*256*256)
	    // Compute the closest color within the original palette
	    for (int k = 0 ; k < size ; k++ ) {
		dist = Color::colorCompare(c, palette[k]);
		if (dist < nearestDist) {
		    nearestDist = dist;
		    nearest = k;
		}
	    }

	    // Check if a mixed color is closer 
	    for (int k = 0 ; k < nbMixed ; k++) {
		dist = Color::colorCompare(c, extendedPalette[k].color);
		if (dist < nearestDist) {
		    nearestDist = dist;
		    isMixed = true;
		    nearest = k;
		}
	    }
      
	    if (isMixed) {
		if ((i + j)%2 == 0) {
		    reconstructed(i, j, 0) = extendedPalette[nearest].c1.getR();
		    reconstructed(i, j, 1) = extendedPalette[nearest].c1.getG();
		    reconstructed(i, j, 2) = extendedPalette[nearest].c1.getB();
		} else {
		    reconstructed(i, j, 0) = extendedPalette[nearest].c2.getR();
		    reconstructed(i, j, 1) = extendedPalette[nearest].c2.getG();
		    reconstructed(i, j, 2) = extendedPalette[nearest].c2.getB();
		}
	    } else {
		reconstructed(i, j, 0) = palette[nearest].getR();
		reconstructed(i, j, 1) = palette[nearest].getG();
		reconstructed(i, j, 2) = palette[nearest].getB();
	    }
	}
    }


    reconstructed.display(reconstructed_disp);
    reconstructed.save("reconstructed-bayer.png");
    cin.ignore();
}

int main(int argc, char* argv[]) {

    // The file path
    char * file;
    // The number of colors in the palette. This correspond to the K of
    // the K-mean algorithm.
    int K;

    // If not enough arguments were given when called
    if (argc != 3) {
	// Display an error and exit
	cerr << "Expected : " << argv[0] << " %filepath %colorsCount" << endl;
	exit(1);
    }
  
    // Retrieve the file path argument
    file = argv[1];
    // Retrieve the number of colors argument
    K = atoi(argv[2]);

    // Check that K is between 2 and 65536
    if(K < 2 || K > 65536) {
	// Error and exit
	cerr << "Color Count must be in [2;65536]" << endl;
	exit(1);
    }

    generatePalette(file, K);

    return 0;
}
