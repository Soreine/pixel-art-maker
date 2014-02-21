#include "ColorHist.h"
#include <iostream>

int main() {
  
  ColorHist ch;
  
  std::cout << ch.getColor(3, 3, 3) << std::endl;

  ch.addColor(Color(1,1,1));
  
  std::cout << ch.getColor(Color(1,1,1)) << std::endl;
  return 0;
}
