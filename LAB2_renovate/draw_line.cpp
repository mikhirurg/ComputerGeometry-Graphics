//
// Created by @mikhirurg on 11.04.2020.
//
#include <iostream>
#include "CImageIO.cpp"

int main() {
    CImageIO<CMonoPixel> img("out.pgm");
    try {
        img.putPixel(1, 3, {100});
        img.writeImg("out.pgm");
    } catch (CImageException e) {
        std::cerr << e.getErr() << std::endl;
    }
    return 0;
}