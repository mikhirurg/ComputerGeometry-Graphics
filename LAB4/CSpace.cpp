//
// Created by @mikhirurg on 06.06.2020.
//
#include <map>
#include <cmath>
#include "CSpace.h"

std::map<const std::string, CSpace *> CSpace::cs_map;

CSpace &CSpace::CSpaceByName(const std::string &name) {
  if (name == "RGB") {
    return *(cs_map[name] = new RGB());
  } else if (name == "HSV") {
    return *(cs_map[name] = new HSV());
  } else if (name == "HSL") {
    return *(cs_map[name] = new HSL());
  } else if (name == "CMY") {
    return *(cs_map[name] = new CMY());
  } else if (name == "YCoCg") {
    return *(cs_map[name] = new YCoCg());
  } else if (name == "YCbCr.601") {
    return *(cs_map[name] = new YCbCr601());
  } else if (name == "YCbCr.709") {
    return *(cs_map[name] = new YCbCr709());
  }
  throw CSpaceException("Invalid colorspace " + name);
}

double Clamp(double a) {
  return std::fmin(std::fmax(a, 0.0), 1.0);
}

CColorPixel HSV::FromRGB(CColorPixel c) {
  CColorPixelDbl cd =
      HSV::FromRGB(CColorPixelDbl{c.r / 255.0, c.g / 255.0, c.b / 255.0});
  return {(uchar) (cd.h * 255), (uchar) (cd.s * 255), (uchar) (cd.v * 255)};
}

CColorPixel HSV::ToRGB(CColorPixel c) {
  CColorPixelDbl cd =
      HSV::ToRGB(CColorPixelDbl{c.h / 255.0, c.s / 255.0, c.v / 255.0});
  return {(uchar) (cd.r * 255), (uchar) (cd.g * 255), (uchar) (cd.b * 255)};
}

CColorPixelDbl HSV::FromRGB(CColorPixelDbl c) {
  double R = c.r, G = c.g, B = c.b;
  double c_high = std::fmax(R, std::fmax(G, B));
  double c_low = std::fmin(R, std::fmin(G, B));
  double c_range = c_high - c_low;
  double H = 0, S = 0, V;
  double c_max = 255.0;

  V = c_high;

  if (V == 0) {
    S = 0;
  } else {
    S = c_range / V;
  }

  if (c_range > 0) {
    if (V == R) {
      H = 60 * (G - B) / c_range;
    } else if (V == G) {
      H = 60 * (2 + (B - R) / c_range);
    } else if (V == B) {
      H = 60 * (4 + (R - G) / c_range);
    }
  }
  return {H / 360.0, S, V};
}

CColorPixelDbl HSV::ToRGB(CColorPixelDbl c) {
  double H = c.h * 360, S = c.s, V = c.v;

  double C = S * V;

  double h_s = H / 60;

  double X = C * (1 - std::fabs(std::fmod(h_s, 2) - 1));

  double R1 = 0, G1 = 0, B1 = 0;

  if (h_s >= 0 && h_s <= 1) {
    R1 = C;
    G1 = X;
    B1 = 0;
  } else if (h_s > 1 && h_s <= 2) {
    R1 = X;
    G1 = C;
    B1 = 0;
  } else if (h_s > 2 && h_s <= 3) {
    R1 = 0;
    G1 = C;
    B1 = X;
  } else if (h_s > 3 && h_s <= 4) {
    R1 = 0;
    G1 = X;
    B1 = C;
  } else if (h_s > 4 && h_s <= 5) {
    R1 = X;
    G1 = 0;
    B1 = C;
  } else if (h_s > 5 && h_s <= 6) {
    R1 = C;
    G1 = 0;
    B1 = X;
  }

  double m = V - C;

  return {R1 + m, G1 + m, B1 + m};
}

CColorPixelDbl HSL::FromRGB(CColorPixelDbl c) {
  double R = c.r, G = c.g, B = c.b;
  double c_high = std::fmax(R, std::fmax(G, B));
  double c_low = std::fmin(R, std::fmin(G, B));
  double c_range = c_high - c_low;
  double H = 0, S = 0, V;
  double c_max = 255.0;

  V = c_high;

  double L = V - c_range / 2.0;

  if (L == 0 || L == 1) {
    S = 0;
  } else {
    S = (V - L) / (std::fmin(L, 1 - L));
  }

  if (c_range > 0) {
    if (V == R) {
      H = 60 * (G - B) / c_range;
    } else if (V == G) {
      H = 60 * (2 + (B - R) / c_range);
    } else if (V == B) {
      H = 60 * (4 + (R - G) / c_range);
    }
  }
  return {H / 360.0, S, L};
}

CColorPixelDbl HSL::ToRGB(CColorPixelDbl c) {
  double H = c.h * 360, S = c.s, L = c.l;

  double C = (1 - std::fabs(2 * L - 1)) * S;

  double h_s = H / 60.0;

  double X = C * (1 - std::fabs(std::fmod(h_s, 2) - 1));

  double R1 = 0, G1 = 0, B1 = 0;

  if (h_s >= 0 && h_s <= 1) {
    R1 = C;
    G1 = X;
    B1 = 0;
  } else if (h_s > 1 && h_s <= 2) {
    R1 = X;
    G1 = C;
    B1 = 0;
  } else if (h_s > 2 && h_s <= 3) {
    R1 = 0;
    G1 = C;
    B1 = X;
  } else if (h_s > 3 && h_s <= 4) {
    R1 = 0;
    G1 = X;
    B1 = C;
  } else if (h_s > 4 && h_s <= 5) {
    R1 = X;
    G1 = 0;
    B1 = C;
  } else if (h_s > 5 && h_s <= 6) {
    R1 = C;
    G1 = 0;
    B1 = X;
  }

  double m = L - C / 2.0;

  return {R1 + m, G1 + m, B1 + m};
}

CColorPixel HSL::FromRGB(CColorPixel c) {
  CColorPixelDbl cd =
      HSL::FromRGB(CColorPixelDbl{c.r / 255.0, c.g / 255.0, c.b / 255.0});
  return {uchar(cd.h * 255), uchar(cd.s * 255), uchar(cd.v * 255)};
}
CColorPixel HSL::ToRGB(CColorPixel c) {
  CColorPixelDbl cd =
      HSL::ToRGB(CColorPixelDbl{c.h / 255.0, c.s / 255.0, c.l / 255.0});
  return {uchar(cd.r * 255), uchar(cd.g * 255), uchar(cd.b * 255)};
}
CColorPixel CMY::FromRGB(CColorPixel c) {
  return {uchar(255 - c.r), uchar(255 - c.g), uchar(255 - c.b)};
}
CColorPixel CMY::ToRGB(CColorPixel c) {
  return {uchar(255 - c.c), uchar(255 - c.m), uchar(255 - c.y)};
}
CColorPixelDbl CMY::FromRGB(CColorPixelDbl c) {
  return {1 - c.r, 1 - c.g, 1 - c.b};
}
CColorPixelDbl CMY::ToRGB(CColorPixelDbl c) {
  return {1 - c.c, 1 - c.m, 1 - c.y};
}

CColorPixel YCbCr601::FromRGB(CColorPixel c) {
  CColorPixelDbl cd = YCbCr601::FromRGB(CColorPixelDbl{c.r / 255.0, c.g / 255.0, c.b / 255.0});
  return {uchar(cd.Y * 255), uchar(cd.Cb * 255), uchar(cd.Cr * 255)};
}
CColorPixelDbl YCbCr601::FromRGB(CColorPixelDbl c) {
  double K_ry = 0.299;
  double K_by = 0.114;
  double K_gy = 1.0 - K_ry - K_by;
  double Y = K_ry * c.r + K_gy * c.g + K_by * c.b;
  double Pb = ((c.b - Y) / (1.0 - K_by)) / 2.0;
  double Pr = ((c.r - Y) / (1.0 - K_ry)) / 2.0;
  return CColorPixelDbl{Y, Pb + 0.5, Pr + 0.5};
}
CColorPixelDbl YCbCr601::ToRGB(CColorPixelDbl c) {
  double K_ry = 0.299;
  double K_by = 0.114;
  double K_gy = 1.0 - K_ry - K_by;
  c.Cb -= 0.5;
  c.Cr -= 0.5;
  double R = c.Y + (2 - 2 * K_ry) * c.Cr;
  double G = c.Y - (K_by * c.Cb) / K_gy * (2.0 - 2.0 * K_by) - K_ry / K_gy * (2.0 - 2.0 * K_ry) * c.Cr;
  double B = c.Y + (2.0 - 2.0 * K_by) * c.Cb;
  return CColorPixelDbl{Clamp(R), Clamp(G), Clamp(B)};
}
CColorPixel YCbCr601::ToRGB(CColorPixel c) {
  CColorPixelDbl cd = YCbCr601::ToRGB(CColorPixelDbl{c.Y / 255.0, c.Cb / 255.0, c.Cr / 255.0});
  return {uchar(cd.r * 255), uchar(cd.g * 255), uchar(cd.b * 255)};
}

CColorPixelDbl YCoCg::FromRGB(CColorPixelDbl c) {
  double Y = c.r / 4.0 + c.g / 2.0 + c.b / 4.0;
  double Co = c.r / 2.0 - c.b / 2.0;
  double Cg = -c.r / 4.0 + c.g / 2.0 - c.b / 4.0;
  return {Y, Co + 0.5, Cg + 0.5};
}

CColorPixelDbl YCoCg::ToRGB(CColorPixelDbl c) {
  double tmp = c.Y - c.Cg + 0.5;
  double R = tmp + c.Co - 0.5;
  double G = c.Y + c.Cg - 0.5;
  double B = tmp - c.Co + 0.5;
  return {Clamp(R), Clamp(G), Clamp(B)};
}

CColorPixel YCoCg::FromRGB(CColorPixel c) {
  CColorPixelDbl cd =
      YCoCg::FromRGB(CColorPixelDbl{c.r / 255.0, c.g / 255.0, c.b / 255.0});
  return {uchar(cd.Y * 255), uchar(cd.Co * 255), uchar(cd.Cg * 255)};
}

CColorPixel YCoCg::ToRGB(CColorPixel c) {
  CColorPixelDbl cd =
      YCoCg::ToRGB(CColorPixelDbl{c.Y / 255.0, c.Co / 255.0, c.Cg / 255.0});
  return {uchar(cd.r * 255), uchar(cd.g * 255), uchar(cd.b * 255)};
}

CColorPixel YCbCr709::FromRGB(CColorPixel c) {
  CColorPixelDbl cd = YCbCr709::FromRGB(CColorPixelDbl{c.r / 255.0, c.g / 255.0, c.b / 255.0});
  return {uchar(cd.Y * 255.0), uchar(cd.Cb * 255.0), uchar(cd.Cr * 255.0)};
}

CColorPixel YCbCr709::ToRGB(CColorPixel c) {
  CColorPixelDbl cd = YCbCr709::ToRGB(CColorPixelDbl{c.Y / 255.0, c.Cb / 255.0, c.Cr / 255.0});
  return {uchar(cd.r * 255), uchar(cd.g * 255), uchar(cd.b * 255)};
}
CColorPixelDbl YCbCr709::FromRGB(CColorPixelDbl c) {
  double K_ry = 0.2126;
  double K_by = 0.0722;
  double K_gy = 1 - K_ry - K_by;
  double Y = K_ry * c.r + K_gy * c.g + K_by * c.b;
  double Pb = ((c.b - Y) / (1.0 - K_by)) / 2.0;
  double Pr = ((c.r - Y) / (1.0 - K_ry)) / 2.0;
  return CColorPixelDbl{Y, Pb + 0.5, Pr + 0.5};
}
CColorPixelDbl YCbCr709::ToRGB(CColorPixelDbl c) {
  double K_ry = 0.2126;
  double K_by = 0.0722;
  double K_gy = 1 - K_ry - K_by;
  c.Cb -= 0.5;
  c.Cr -= 0.5;
  double R = c.Y + (2 - 2 * K_ry) * c.Cr;
  double G = c.Y - (K_by * c.Cb) / K_gy * (2.0 - 2.0 * K_by) - K_ry / K_gy * (2.0 - 2.0 * K_ry) * c.Cr;
  double B = c.Y + (2.0 - 2.0 * K_by) * c.Cb;
  return CColorPixelDbl{Clamp(R), Clamp(G), Clamp(B)};
}
