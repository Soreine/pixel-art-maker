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
    This program loads a palette image and an image and then try to
    display this image using the computed palette and a given
    threshold matrix.
    Use : 'dither %paletteFile %thresholdPattern %imageFile (%outputFile)'
    Parameters :
    paletteFile : The path to the palette file.
    thresholdPattern : The path to an image used as a threshold matrix
    imageFile : The path to the image file to dither
    outputFile : The result image will be saved under this name
    Result :
    The reconstructed image will be saved.
*/


inline Color getPixel(int const x, int const y, CImg<unsigned char> const& image) {
    return Color(image(x, y, 0), image(x, y, 1), image(x, y, 2));
}


/** Take all the pixels from an image and save their color in a Color
    list. Return the number of colors found */

/** Return an non dithered image, made with the input palette */
CImg<unsigned char>  nodither(CImg<unsigned char> image, CImg<unsigned char> palette) {
    /*
    // The number of colors in the palette
    
    // Grab the colors from the palette image
    Color * paletteColors = grabColors(palette);
    /////////////// Reconstruction ///////////////////
    CImg<unsigned char> reconstructed(image.width(), image.height(), 1, 3, 0);
    CImgDisplay reconstructed_disp(reconstructed, "Image reconstruite");
    Color c;
    int nearest;
    double nearestDist, dist;
    // The number of colors in the palette.
    int paletteCount;

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
    */
    return CImg<unsigned char>(100, 100, 1, 3, 0);
}

int main(int argc, char* argv[]) {

    // The image file path
    char * fileImage;
    // The palette image file path
    char * filePalette;
    // The threshold pattern image file path
    char * fileThreshold;

    // If not enough arguments were given when called
    if (argc != 4) {
	// Display an error and exit
	cerr << "Expected : " << argv[0] << " %imageFile %paletteFile %thresholdFile" << endl;
	exit(1);
    }
  
    fileImage = argv[1];
    filePalette = argv[2];
    fileThreshold = argv[3];

    // The image
    CImg<unsigned char>  image(fileImage);
    // The palette image
    CImg<unsigned char>  palette(filePalette);
    // The threshold image
    CImg<unsigned char>  threshold(fileThreshold);

    // The dithered result image
    CImg<unsigned char>  result(fileThreshold);

    palette.draw_image(0,0,0,0,img,img.get_channel(3),1,255);

    for(int i = 0; i < palette.width(); i++) {
	for(int j = 0; j < palette.height(); j++) {
	    cout <<  palette(i, j, 0) << " " << palette(j, i, 1) 
		 << " " << palette(i, j, 2) << " " << palette(j, i, 3) << endl;
	}		     
    }
    
    /*
    result = nodither(image, palette, threshold);

    // Save dithered image
    if(argc >= 5) {
	result.save(outputFile);
    } else {
	// Create a generic file name
	string string_output = std::string(filePalette) + "-" 
	    + std::string(fileThreshold) + "-dithered-" + std::string(fileImage);
	const char * char_output = string_palette.c_str();
	result.save(char_output);
	cout << endl << "Palette image saved under " << char_output << endl;
    }
    */
    return 0;
}
