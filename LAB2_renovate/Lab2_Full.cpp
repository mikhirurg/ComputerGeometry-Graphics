//
// Created by @mikhirurg on 11.04.2020.
//
#include <iostream>
#include "CImage.cpp"

int main(int argc, char *argv[]) {
    try {
        if (argc != 10) {
            throw CImageParamsException();
        }
        CImage<CMonoPixel> img = CImage<CMonoPixel>(argv[1]);
        int brightness;
        double thickness, x1, y1, x2, y2, gamma;
        try {
            brightness = std::stoi(argv[3]);
            thickness = std::stod(argv[4]);
            x1 = std::stod(argv[5]);
            y1 = std::stod(argv[6]);
            x2 = std::stod(argv[7]);
            y2 = std::stod(argv[8]);
            gamma = std::stod(argv[9]);
        } catch (std::invalid_argument &) {
            throw CImageParamsException();
        }
        img.drawLine(brightness, thickness, x1, y1, x2, y2, gamma);
        img.writeImg(argv[2]);
    } catch (CImageException e) {
        std::cerr << e.getErr() << std::endl;
        return 1;
    }
    return 0;
}
