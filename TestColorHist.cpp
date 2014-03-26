#include <iostream>
#include "ColorHist.h"
#include "Color.h"

using namespace std;

int main() {
  cout << "Debut";

  cout.flush();
  int a[3][3];
  a[0][0] = 1;
  a[1][0] = 2;
  a[2][0] = 3;

  cout << a[0][0] << " " << a[1][1] << " " << a[2][0] << endl; 
  cout.flush();

  ColorHist ch;

  //cout << ch.getColor(3, 3, 3) << endl;

  ch.addColor(Color(0,0,0));
  
  cout << ch.getColor(Color(0,0,0)) << endl;

  return 0;
}
