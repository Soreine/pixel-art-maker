#include <list>
#include <iostream>
using namespace std;

#include "CImg-1.5.7/CImg.h"
using namespace cimg_library;

#include <string>
#include "lib/ColorHist.h"
#include "lib/Color.h"
#include "lib/Triplet.h"

/*
  use : ditherer %file %nb_color
  
*/

/** This struct represent a point in the RGB space, along with the
    cluster it currently belongs to. */
struct point {
  // The RGB coordinates
  Color color;
  // The cluster index this point belongs to
  unsigned int cluster;
};


/** 
    This program determine the best color palette to use to display an
    image and then try to display this image using the computed
    palette and a given threshold matrix. 
    'ditherer %filepath %colorsCount
    Parameters :
      filepath : The path to the image file.
      colorsCount : The desired number of colors in the palette used
    Result :
      The reconstructed image and an image containing the palette's
      colors will be saved.
*/
int main(int argc, char* argv[]) {

  // The file path
  char * file;
  // The number of colors in the palette. This correspond to the K of
  // the K-mean algorithm.
  int K;
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
  // The centers of the clusters in the k-mean algorithm. At the end :
  // the computed color palette
  Color * kmean;

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

void generatePalette(char * file, int K) {

  // Initialize the means array
  kmean = new Color[K];


  // Load the image
  CImg<unsigned char> image(file);
  // Create the image that will contain the palette colors. 
  CImg<unsigned char> palette(256, 256, 1, 3, 0);
  // Create a display for the original image
  CImgDisplay orig_disp(image, "Original");
  // Create a display for the palette
  CImgDisplay palette_disp(palette, "Resultat");


  // Initialize the color histogram and the point list

  for (int i = 0 ; i < image.height() ; i++) {
    for (int j = 0 ; j < image.width() ; j++) { // For each pixel
      // 
      p.color = getPixel(j, i, image);
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
     cerr << "The original image has less colors than the desired number of colors
 in the palette." << endl;
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
   // The next computed kmeans. Acts as an intermediate buffer 
   Triplet * nextKMean = new Triplet[K];
    
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
	 dist = Color::distance(c, kmean[i]);
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
      
     // Plot the means
     for (int i = 0 ; i < K ; i++ ) {
       c = kmean[i];
       for (int j = i*(image.height()/K); j < (i+1)*(image.height()/K) ; j++) {
	 for (int k = 0 ; k < image.width() ; k++) {
	   palette(k, j, 0) = c.getR();
	   palette(k, j, 1) = c.getG();
	   palette(k, j, 2) = c.getB();
	 }
       }
     }
     palette.display(palette_disp);

     cout << ".";
     cout.flush();
   }

   cout << endl << "Done. Hit Enter to begin reconstruction" << endl;
   cin.ignore();
  
   // Begin reconstructions of the original picture.

   reconstruct_basic(image, kmean, K);

   // reconstruct_alternate(image, kmean, K);

   // reconstruct_bayer(image, kmean, K);

   delete[] nextKMean; // Warning, allows array overhead...
   delete[] kmean;
  
   return 0;
}


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

