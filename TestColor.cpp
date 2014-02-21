#include "Color.h"

#include <iostream>

int main() {
  Color c(234, 220, 10);
  
  std::cout << Color(3,2,1).getR() << std::endl;
  std::cout << c.getG() << std::endl;
  std::cout << c.getB() << std::endl;

  return 0;
}
