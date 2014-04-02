#include <list>
#include <iostream>
using namespace std;

#include "CImg-1.5.7/CImg.h"
using namespace cimg_library;

#include <string>
#include "lib/ColorHist.h"
#include "lib/Color.h"

/*
  pixelArtMaker "file" nb_color
  
*/

class Point {
  Color color;
  unsigned int cluster;
};

inline Color getPixel(unsigned int const x, unsigned int const y, CImg<unsigned char> const& image) {
  return Color(image(x, y, 0), image(x, y, 1), image(x, y, 2));
}

int main(int argc, char* argv[]) {

  char * file;
  unsigned int K;

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

  list<Point> colorList;

  Color kmean[K];
  
  ColorHist ch;
  
  /* Charger l'image */

  CImg<unsigned char> image(file);
  
  Color c;

  /* Remplir l'histogramme et la liste des couleurs. */
  for (int i = 0 ; i < image.height() ; i++) {
    for (int j = 0 ; j < image.width() ; j++) {
      c = getPixel(i, j, image);
      if (!ch.addColor(c))
	colorList.add6
    }
  }


  return 0;
}
