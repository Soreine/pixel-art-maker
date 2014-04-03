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


inline Color getPixel(unsigned int const x, unsigned int const y, CImg<unsigned char> const& image) {
  return Color(image(x, y, 0), image(x, y, 1), image(x, y, 2));
}

int main(int argc, char* argv[]) {

  char * file;
  unsigned int K;

  list<point> pointList;
  Color kmean[K];
  ColorHist ch;
  Color c;
  point p;

  // TODO
  /*  if (argc < 3) {
      file = "lena.jpg";
      K = 16;
      } else {*/
  file = argv[1];
  K = atoi(argv[2]);
  //  }

  cout << endl;
  cout << "============================" << endl;
  cout << "Bienvenue dans PixelArtMaker" << endl;

  /* Charger l'image */

  CImg<unsigned char> image(file);
  CImg<unsigned char> visu(image.width(), image.height(), 1, 3, 0);
  CImgDisplay orig_disp(image, "Original"), visu_disp(visu, "Resultat");



  /* Remplir l'histogramme et la liste des couleurs. */
  for (int i = 0 ; i < image.height() ; i++) {
    for (int j = 0 ; j < image.width() ; j++) {
      p = {getPixel(j, i, image), 0}; //WARNING -std=c++11
      if (!ch.addColor(p.color))
	pointList.push_front(p);
    }
  }

  ////////////////// K-Mean ////////////////////////



  // Initialiser les clusters avec des couleurs de l'image.
  list<point>::iterator iterator = pointList.begin();
  bool enoughColors = true;
  kmean[0] = c = (*iterator).color;
  { 
    int i = 1;
    while (i < K) {
      if (iterator == pointList.end()) {
	enoughColors = false;
	break;
      }
      if (Color::distance((*iterator).color, c) != 0) {
	c = (*iterator).color;
	kmean[i] = c;
	cout << c << endl;
	i++;
      }
      iterator++;
    }
  }


  
  cout << "KMEAN BEFORE" << endl;
  for (int i = 0; i < K; i++) {
    cout << kmean[i] << endl;
  }



  if (enoughColors) {
    // Algorithme des k-moyennes //////////////////////////////
    bool changed = true;
    unsigned int nearest;
    double nearestDist;
    double dist;
    unsigned long clusterWeight[K];
    unsigned long weight;
    Triplet nextKMean[K];

    while(changed) {

      cout << "KMEAN AFTER" << endl;
      for (int i = 0; i < K; i++) {
	cout << kmean[i] << endl;
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

      cout << "Toutes les couleurs traitées" << endl;
	

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
	nextKMean[i].divide(clusterWeight[i]);
	c = nextKMean[i].getColor();
	cout << kmean[i] << " puis " << c << endl;
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
	    visu(k, j, 0) = c.getR();
	    visu(k, j, 1) = c.getG();
	    visu(k, j, 2) = c.getB();
	  }
	}
      }
      visu.display(visu_disp);
      
      cin.ignore();
    }
    
    cout << "Fin de l'algorithme" << endl;
    visu.display(visu_disp);

    cin.ignore();

  } else {
    cout << "L'image originale contient moins de couleur que le nombre demandé." << endl;
  }
  
  return 0;
}
