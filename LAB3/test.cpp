//
// Created by @mikhirurg on 26.04.2020.
//
#include <iostream>
#include <ctime>
#include "CDitherer.h"
#include "CImage.h"
#include "CImageException.h"

int main() {
  try {
    for (int i = 1; i <= 8; i++) {
      CImage<CColorPixel> img = CImage<CColorPixel>("forest_sample.pnm");
      CDitherer<CColorPixel> ditherer = CDitherer<CColorPixel>(img);
      ditherer.DoFloydSteinbergDithering(i);
      img.WriteImg("forest_floyd_sample" + std::to_string(i) + ".pnm");
    }
  } catch (CImageException e) {
    std::cerr << e.getErr();
    return 1;
  }
  return 0;
}