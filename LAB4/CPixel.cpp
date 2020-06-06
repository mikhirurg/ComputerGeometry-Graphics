//
// Created by @mikhirurg on 06.06.2020.
//
#include "CPixel.h"

CColorPixel::operator CColorPixelDbl() {
  return {r/255.0, g/255.0, b/255.0};
}
