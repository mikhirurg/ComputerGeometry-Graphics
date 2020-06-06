//
// Created by @mikhirurg on 06.06.2020.
//

#ifndef LAB4__CSPACE_H_
#define LAB4__CSPACE_H_
#include <string>
#include <map>
#include "CPixel.h"

class CSpaceException {
  std::string err_m_;
 public:
  CSpaceException(const std::string &str) : err_m_(str) {
  }

  operator std::string() {
    return err_m_;
  }
};

class CSpace {
  std::string name;
  static std::map<const std::string, CSpace *> cs_map;
 public:
  CSpace(std::string nm) : name(nm) {}
  virtual CColorPixel FromRGB(CColorPixel c) = 0;
  virtual CColorPixel ToRGB(CColorPixel c) = 0;
  virtual CColorPixelDbl FromRGB(CColorPixelDbl c) = 0;
  virtual CColorPixelDbl ToRGB(CColorPixelDbl c) = 0;

  static CSpace &CSpaceByName(const std::string &name);
};

class RGB : public CSpace {
 public:
  RGB() : CSpace("RGB") {}
  CColorPixel FromRGB(CColorPixel c) override {
    return c;
  }
  CColorPixel ToRGB(CColorPixel c) override {
    return c;
  }
  CColorPixelDbl FromRGB(CColorPixelDbl c) override {
    return c;
  }
  CColorPixelDbl ToRGB(CColorPixelDbl c) override {
    return c;
  }

};

class HSV : public CSpace {
 public:
  HSV() : CSpace("HSV") {}
  CColorPixel FromRGB(CColorPixel c) override;
  CColorPixel ToRGB(CColorPixel c) override;
  CColorPixelDbl FromRGB(CColorPixelDbl c) override;
  CColorPixelDbl ToRGB(CColorPixelDbl c) override;
};

class HSL : public CSpace {
 public:
  HSL() : CSpace("HSL") {}
  CColorPixel FromRGB(CColorPixel c) override;
  CColorPixel ToRGB(CColorPixel c) override;
  CColorPixelDbl FromRGB(CColorPixelDbl c) override;
  CColorPixelDbl ToRGB(CColorPixelDbl c) override;
};

class CMY : public CSpace {
 public:
  CMY() : CSpace("CMY") {}
  CColorPixel FromRGB(CColorPixel c) override;
  CColorPixel ToRGB(CColorPixel c) override;
  CColorPixelDbl FromRGB(CColorPixelDbl c) override;
  CColorPixelDbl ToRGB(CColorPixelDbl c) override;
};

class YCoCg : public CSpace {
 public:
  YCoCg() : CSpace("YCoCg") {}
  CColorPixel FromRGB(CColorPixel c) override;
  CColorPixel ToRGB(CColorPixel c) override;
  CColorPixelDbl FromRGB(CColorPixelDbl c) override;
  CColorPixelDbl ToRGB(CColorPixelDbl c) override;
};

class YCbCr601 : public CSpace {
 public:
  YCbCr601() : CSpace("YCbCr.601") {}
  CColorPixel FromRGB(CColorPixel c) override;
  CColorPixel ToRGB(CColorPixel c) override;
  CColorPixelDbl FromRGB(CColorPixelDbl c) override;
  CColorPixelDbl ToRGB(CColorPixelDbl c) override;
};

class YCbCr709 : public CSpace {
 public:
  YCbCr709() : CSpace("YCbCr.709") {}
  CColorPixel FromRGB(CColorPixel c) override;
  CColorPixel ToRGB(CColorPixel c) override;
  CColorPixelDbl FromRGB(CColorPixelDbl c) override;
  CColorPixelDbl ToRGB(CColorPixelDbl c) override;
};

#endif //LAB4__CSPACE_H_
