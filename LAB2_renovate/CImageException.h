//
// Created by @mikhirurg on 11.04.2020.
//

#ifndef COMPUTERGEOMETRY_GRAPHICS_CIMAGEEXCEPTION_H
#define COMPUTERGEOMETRY_GRAPHICS_CIMAGEEXCEPTION_H

#include <string>

class CImageException {
 public:
  CImageException(const std::string &err_m);

  const char *getErr();

 private:
  std::string err_m_;
};

#endif //COMPUTERGEOMETRY_GRAPHICS_CIMAGEEXCEPTION_H
