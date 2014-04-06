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
  CImg<unsigned char> visu(image.width(), image.height(), 1, 3, 0);
  CImgDisplay orig_disp(image, "Original"), visu_disp(visu, "Resultat");



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



  // Initialiser les clusters avec des couleurs de l'image.
  list<point>::iterator iterator = pointList.begin();
  bool enoughColors = true;
  kmean[0] = c = (*iterator).color;
  for (int i = 1; i < K;) {
    if (iterator == pointList.end()) {
      enoughColors = false;
      break;
    }
    if (Color::distance((*iterator).color, c) != 0) {
      c = (*iterator).color;
      
      kmean[i] = c;
      //cout << c << endl;
      i++;
    }
    iterator++;
  }

  
  //cout << "KMEAN BEFORE" << endl;
  for (int i = 0; i < K; i++) {
    //cout << kmean[i] << endl;
  }
  
  

  if (not enoughColors) {
    cerr << "L'image originale contient moins de couleur que le nombre demandé." << endl;
    exit(1);
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
	  visu(k, j, 0) = c.getR();
	  visu(k, j, 1) = c.getG();
	  visu(k, j, 2) = c.getB();
	}
      }
    }
    visu.display(visu_disp);

    cout << ".";
    cout.flush();
  }

  cout << endl << "Done" << endl;
  cin.ignore();

  delete[] nextKMean;
  delete[] kmean;
  
  return 0;
}
