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
  pixelArtMaker "file" nb_color
  
*/

struct point {
  Color color;
  unsigned int cluster;
};


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
	dist = Color::distance(c, palette[k]);
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

void reconstruct_alternate(CImg<unsigned char> image, Color * palette, int size) {

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
      extendedPalette[k].color = palette[i].mix(palette[j], 0.5); // The half mix between i and j
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
	dist = Color::distance(c, palette[k]);
	if (dist < nearestDist) {
	  nearestDist = dist;
	  nearest = k;
	}
      }

      // Check if a mixed color is closer 
      for (int k = 0 ; k < nbMixed ; k++) {
	dist = Color::distance(c, extendedPalette[k].color);
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
  reconstructed.save("reconstructed-alternate.png");
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
      extendedPalette[k].color = palette[i].mix(palette[j], 0.5); // The half mix between i and j
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
	dist = Color::distance(c, palette[k]);
	if (dist < nearestDist) {
	  nearestDist = dist;
	  nearest = k;
	}
      }

      // Check if a mixed color is closer 
      for (int k = 0 ; k < nbMixed ; k++) {
	dist = Color::distance(c, extendedPalette[k].color);
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

  char * file;
  int K;

  list<point> pointList;
  ColorHist ch;
  Color c;
  point p;
  Color * kmean;

  if (argc != 3) {
    cerr << "Usage: " << argv[0] << " picture.jpg numberOfColor" << endl;
    exit(1);
  }
  
  file = argv[1];
  K = atoi(argv[2]);

  kmean = new Color[K];

  cout << endl;
  cout << "============================" << endl;
  cout << "Bienvenue dans PixelArtMaker" << endl;

  /* Charger l'image */

  CImg<unsigned char> image(file);
  CImg<unsigned char> palette(image.width(), image.height(), 1, 3, 0);
  CImgDisplay orig_disp(image, "Original"), palette_disp(palette, "Resultat");



  /* Remplir l'histogramme et la liste des couleurs. */
  for (int i = 0 ; i < image.height() ; i++) {
    for (int j = 0 ; j < image.width() ; j++) {
      p.color = getPixel(j, i, image);
      p.cluster = 0; 
      if (!ch.addColor(p.color))
	pointList.push_front(p);
    }
  }
  
  ////////////////// K-Mean ////////////////////////

  if (pointList.size() < (unsigned int) K) { // Pas assez de couleurs
    cerr << "L'image originale contient moins de couleur que le nombre demandé." << endl;
    exit(1);
  }

  // Initialiser les clusters avec des couleurs de l'image.
  list<point>::iterator iterator = pointList.begin();
  
  unsigned int step = pointList.size()/(unsigned int) K;

  for (int i = 0; i < K; i++) {
      c = (*iterator).color;
      kmean[i] = c;

      std::advance(iterator, step);
  }

  
  // Algorithme des k-moyennes //////////////////////////////
    
  bool changed = true;
  int nearest;
  double nearestDist;
  double dist;
  long * clusterWeight = new long[K];
  long weight;
  Triplet * nextKMean = new Triplet[K];
    
  while(changed) {
      
    //cout << "KMEAN AFTER" << endl;
    for (int i = 0; i < K; i++) {
      //cout << kmean[i] << endl;
    }
      
    for (int i = 0 ; i < K ; i++ ) {
      nextKMean[i] = Triplet(0,0,0);
      clusterWeight[i] = 0;
    }
      
    // Mettre à jour les clusters
    for (list<point>::iterator iterator = pointList.begin() ; 
	 iterator != pointList.end() ; iterator++) {
      c = (*iterator).color;
	
      nearest = 0;
      nearestDist = 4096; // sqrt(256*256*256)
      // Calculer la moyenne la plus proche
      for (int i = 0 ; i < K ; i++ ) {
	  
	dist = Color::distance(c, kmean[i]);
	if (dist < nearestDist) {
	  nearestDist = dist;
	  nearest = i;
	}
      }
	
      (*iterator).cluster = nearest;
	
      // Calculer les prochaines moyennes
      weight = ch.getColor(c);
	
      nextKMean[nearest].addMultiply(c, weight);
      clusterWeight[nearest] += weight;
	
    }
      
    //cout << "Toutes les couleurs traitées" << endl;
      
      
    /*
      for (list<point>::iterator iterator = pointList.begin() ; 
      iterator != pointList.end() ; iterator++) {
	
      // Mettre à jour les moyennes    
	
      c = (*iterator).color;
      m = ch.getColor(c);
	
      nextKMean[(*iterator).cluster].addMultiply(c, m);
      clusterWeight[(*iterator).cluster] += m;
      }
    */
      
      
    // Diviser les moyennes par leur poids.  Mettre à jour les
    // moyennes et vérifier si elles ont changé.
      
    changed = false;
    for (int i = 0 ; i < K ; i++ ) {
      if(clusterWeight[i] != 0) 
	nextKMean[i].divide(clusterWeight[i]);
      c = nextKMean[i].getColor();
      // cout << kmean[i] << " puis " << c << endl;
      changed = changed or
	(c.getR() != kmean[i].getR()) or
	(c.getG() != kmean[i].getG()) or
	(c.getB() != kmean[i].getB());
	
      kmean[i] = c;
    }
      
    // Afficher les moyennes
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
  
  reconstruct_basic(image, kmean, K);

  reconstruct_alternate(image, kmean, K);

  reconstruct_bayer(image, kmean, K);

  delete[] nextKMean; // Attention, permet les dépassement de tableau
  delete[] kmean;
  
  return 0;
}
