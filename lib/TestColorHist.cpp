#include <iostream>
#include <assert.h>
#include <math.h>
#include "ColorHist.h"
#include "Color.h"

using namespace std;

int main() {

  cout << "Test Color" << endl;

  Color c;

  c = Color(5,6,7);

  assert(Color::distance(c, Color(2,4,6))==sqrt(14));


  cout << "Test ColorHist" << endl;

  ColorHist ch;

  // cout << "Il y a " << ch.getColor(3, 3, 3) << " couleur 3,3,3." << endl;
  assert(ch.getColor(3,3,3)==0);

  // cout << "Ajout d'une couleur 0,0,0" << endl;
  ch.addColor(Color(0,0,0));
  
  // cout << "Il y a " << ch.getColor(Color(0,0,0)) << " couleur 0,0,0." << endl;

  assert(ch.getColor(0,0,0)==1);
  assert(ch.getColor(Color(0,0,0))==1);

  cout << "Color(1,2,3)" << endl;
  cout << Color(1,2,3) << endl;

  return 0;
}
