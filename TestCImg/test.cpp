#include "../CImg-1.5.7/CImg.h"
using namespace cimg_library;

int main() {

  CImg<unsigned char> image("lena.bmp"), visu(500,400,1,3,0);

  CImgDisplay main_disp(image,"Lena"), draw_disp(visu,"My Image"); 
  
  const unsigned char red[] = {255, 0, 0};

  while(!main_disp.is_closed() && !draw_disp.is_closed()) {
      main_disp.wait();
      if (main_disp.button() && main_disp.mouse_y()>=0) {
	const int x = main_disp.mouse_x();
	const int y = main_disp.mouse_y();
	visu(x, y, 0) = image(x, y, 0);
	visu(x, y, 1) = image(x, y, 1);
	visu(x, y, 2) = image(x, y, 2);
	visu.display(draw_disp);
      }
    }

  visu.save("result.png");
}
