#include "../CImg-1.5.7/CImg.h"
using namespace cimg_library;

int main() {

  CImg<unsigned char> image("lena.bmp"), visu(500,400,1,3,0);

  CImgDisplay main_disp(image,"Click a point"), draw_disp(visu,"My Image"); 
  
  while(!main_disp.is_closed() && !draw_disp.is_closed()) {
      main_disp.wait();
      draw_disp.wait();
    }
}
