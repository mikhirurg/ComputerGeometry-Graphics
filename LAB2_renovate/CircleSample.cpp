//
// Created by @mikhirurg on 11.04.2020.
//
#include <iostream>
#include <cmath>
#include "CImage.cpp"

int main() {
  try {
    double x0 = 300;
    double y0 = 200;
    double len = 100;
    double gamma = 2.2;
    for (double deg = 0; deg < 360; deg += 1.0) {
      CImage<CMonoPixel> img("img/test.pgm");
      double x1 = x0 + len * cos(deg * 3.1415 / 180.0);
      double y1 = y0 - len * sin(deg * 3.1415 / 180.0);
      img.drawLine(255, 50, x0, y0, x1, y1, gamma);
      img.writeImg("img/out" + std::to_string((int) deg) + ".pgm");
      std::system(("magick convert img/out" + std::to_string((int) deg) + ".pgm img/out" + std::to_string((int) deg)
          + ".png").c_str());
      std::cout << std::to_string(deg) << std::endl;
    }
    std::system("magick convert img/out{0..359}.png img/out.gif");
  } catch (CImageException e) {
    std::cerr << e.getErr() << std::endl;
  }
  return 0;
}