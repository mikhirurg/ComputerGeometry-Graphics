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
    /*std::string fname = "michelangelo";
    for (int i = 1; i <= 8; i++) {
      CImage<CMonoPixel> img = CImage<CMonoPixel>(fname+".pgm");
      CDitherer<CMonoPixel> ditherer = CDitherer<CMonoPixel>(img);
      ditherer.DoJJNDithering(i);
      img.WriteImg(fname+"_old_jjn_sample"+std::to_string(i)+".pgm");
    }
    for (int i = 1; i <= 8; i++) {
      CImage<CMonoPixel> img = CImage<CMonoPixel>(fname+".pgm");
      CDitherer<CMonoPixel> ditherer = CDitherer<CMonoPixel>(img);
      ditherer.DoErrorDiffDithering(ditherer.JJN, i);
      img.WriteImg(fname+"_new_jjn_sample"+std::to_string(i)+".pgm");
    }*/
    CImage<CMonoPixel> img = CImage<CMonoPixel>("1_0_1_1.0.pgm", 2.2);
    img.FillWithGradient();
    CDitherer<CMonoPixel> ditherer = CDitherer<CMonoPixel>(img);
    //ditherer.DoOrderedDithering(ditherer.SAMPLE_BAYER8);
    //ditherer.DoOrderedDithering(ditherer.SAMPLE_BAYER8, 2);
    //ditherer.DoRandomDithering(1, time(0));
    ditherer.DoColorBitCorrection(1);
    img.CorrectImageWithGamma();
    img.WriteImg("outp.pgm");
  } catch (CImageException e) {
    std::cerr << e.getErr();
    return 1;
  }
  return 0;
}