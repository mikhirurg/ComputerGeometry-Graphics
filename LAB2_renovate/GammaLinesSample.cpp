//
// Created by @mikhirurg on 25.04.2020.
//
#include <iostream>
#include "CImage.cpp"


int main() {
  try {
    CImage<CMonoPixel> img("test.pgm");
    int bright = 180;
    img.drawLine(128, 25, 0, 25, 100, 0, 2.2);
    img.drawLine(128, 25, 0, 75, 100, 50, 1);
    img.writeImg("out.pgm");
  } catch (CImageException e) {
    std::cerr << e.getErr();
  }
}
