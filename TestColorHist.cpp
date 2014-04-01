#include <iostream>
#include "assert.h"
#include "ColorHist.h"
#include "Color.h"

using namespace std;

int main() {

  ColorHist ch;

  cout << "Il y a " << ch.getColor(3, 3, 3) << " couleur 3,3,3." << endl;
  assert(ch.getColor(3,3,3)==0);

  cout << "Ajout d'une couleur 0,0,0" << endl;
  ch.addColor(Color(0,0,0));
  
  cout << "Il y a " << ch.getColor(Color(0,0,0)) << " couleur 0,0,0." << endl;

  assert(ch.getColor(0,0,0)==1);
  assert(ch.getColor(Color(0,0,0))==1);

  int x;
  cin >> x;

  return 0;
}
