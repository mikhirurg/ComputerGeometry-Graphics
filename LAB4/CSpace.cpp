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
  double R = c.r * 255.0, G = c.g * 255.0, B = c.b * 255.0;
  double c_high = std::fmax(R, std::fmax(G, B));
  double c_low = std::fmin(R, std::fmin(G, B));
  double c_rng = c_high - c_low;
  double H = 0, S = 0, V;
  double c_max = 255.0;

  V = c_high / c_max;

  if (c_high > 0) {
    S = c_rng / c_high;
  }

  if (c_rng > 0) {
    double rr = (c_high - R) / c_rng;
    double gg = (c_high - G) / c_rng;
    double bb = (c_high - B) / c_rng;
    double hh;
    if (R == c_high) {
      hh = bb - gg;
    } else if (G == c_high) {
      hh = rr - bb + 2.0;
    } else {
      hh = gg - rr + 4.0;
    }
    if (hh < 0) {
      hh = hh + 6.0;
    }
    H = hh / 6.0;
  }
  return {H, S, V};
}

CColorPixelDbl HSV::ToRGB(CColorPixelDbl c) {
  double H = c.h, S = c.s, V = c.v;
  double r = 0, g = 0, b = 0;
  double hh = ((long) (6 * H)) % 6;
  int c1 = (long) hh;
  double c2 = hh - c1;
  double x = (1 - S) * V;
  double y = (1 - (S * c2)) * V;
  double z = (1 - (S * (1 - c2))) * V;
  switch (c1) {
    case 0:r = V;
      g = z;
      b = x;
      break;
    case 1:r = y;
      g = V;
      b = x;
      break;
    case 2:r = x;
      g = V;
      b = z;
      break;
    case 3:r = x;
      g = y;
      b = V;
      break;
    case 4:r = z;
      g = x;
      b = V;
      break;
    case 5:r = V;
      g = x;
      b = y;
      break;
  }
  double R = std::fmin(r, 1.0);
  double G = std::fmin(g, 1.0);
  double B = std::fmin(b, 1.0);
  return {R, G, B};
}

CColorPixelDbl HSL::FromRGB(CColorPixelDbl c) {
  double R = c.r * 255.0, G = c.g * 255.0, B = c.b * 255.0;
  double c_high = std::fmax(R, std::fmax(G, B));
  double c_low = std::fmin(R, std::fmin(G, B));
  double c_rng = c_high - c_low;

  double L = ((c_high + c_low) / 255.0) / 2.0;

  double S = 0;
  if (0 < L && L < 1) {
    double d = (L <= 0.5) ? L : (1 - L);
    S = 0.5 * (c_rng / 255.0) / d;
  }

  double H = 0;
  if (c_high > 0 && c_rng > 0) {
    double r = double(c_high - R) / c_rng;
    double g = double(c_high - G) / c_rng;
    double b = double(c_high - B) / c_rng;
    double h;
    if (R == c_high) {
      h = b - g;
    } else if (G == c_high) {
      h = r - b + 2.0;
    } else {
      h = g - r + 4.0;
    }
    if (h < 0) {
      h = h + 6.0;
    }
    H = h / 6;
  }
  return {H, S, L};
}

CColorPixelDbl HSL::ToRGB(CColorPixelDbl c) {
  double H = c.h, L = c.l, S = c.s;
  double r = 0, g = 0, b = 0;
  if (L <= 0) {
    r = g = b = 0;
  } else if (L >= 1) {
    r = g = b = 1;
  } else {
    double hh = long(6 * H) % 6;
    int c1 = hh;
    double c2 = hh - c1;
    double d = (L <= 0.5) ? (S * L) : S * (1 - L);
    double w = L + d;
    double x = L - d;
    double y = w - (w - x) * c2;
    double z = x + (w - x) * c2;
    switch (c1) {
      case 0: r = w;
        g = z;
        b = x;
        break;
      case 1: r = y;
        g = w;
        b = x;
        break;
      case 2: r = x;
        g = w;
        b = z;
        break;
      case 3: r = x;
        g = y;
        b = w;
        break;
      case 4: r = z;
        g = x;
        b = w;
        break;
      case 5: r = w;
        g = x;
        b = y;
        break;
    }
  }
  double R = std::fmin(r, 1.0);
  double G = std::fmin(g, 1.0);
  double B = std::fmin(b, 1.0);

  return {R, G, B};
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
  double K_gy = 1 - K_ry - K_by;
  double Y = K_ry * c.r + K_gy * c.g + K_by * c.b;
  double Cb = c.b - Y;
  double Cr = c.r - Y;
  return CColorPixelDbl{Clamp(Y), Clamp(Cb), Clamp(Cr)};
}
CColorPixelDbl YCbCr601::ToRGB(CColorPixelDbl c) {
  double K_ry = 0.299;
  double K_by = 0.114;
  double K_gy = 1 - K_ry - K_by;
  double R = c.Y + c.Cr;
  double G = c.Y - (K_by / K_gy) * c.Cb - (K_ry / K_gy) * c.Cr;
  double B = c.Y + c.Cb;
  return CColorPixelDbl{Clamp(R), Clamp(G), Clamp(B)};
}
CColorPixel YCbCr601::ToRGB(CColorPixel c) {
  CColorPixelDbl cd = YCbCr601::ToRGB(CColorPixelDbl{c.Y / 255.0, c.Cb / 255.0, c.Cr / 255.0});
  return {uchar(cd.r * 255), uchar(cd.g * 255), uchar(cd.b * 255)};
}

CColorPixelDbl YCoCg::FromRGB(CColorPixelDbl c) {
  double Y = c.r / 4.0 + c.g / 2.0 + c.b / 4.0;
  double Co = c.r / 2.0 - c.b / 2.0;
  double Cg = -c.r / 4.0 + c.g / 2 - c.b / 4.0;
  return {Clamp(Y), Clamp(Co), Clamp(Cg)};
}

CColorPixelDbl YCoCg::ToRGB(CColorPixelDbl c) {
  double tmp = c.Y - c.Cg;
  double R = tmp + c.Co;
  double G = c.Y + c.Cg;
  double B = tmp - c.Co;
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
  double Cb = c.b - Y;
  double Cr = c.r - Y;
  return CColorPixelDbl{Clamp(Y), Clamp(Cb), Clamp(Cr)};
}
CColorPixelDbl YCbCr709::ToRGB(CColorPixelDbl c) {
  double K_ry = 0.2126;
  double K_by = 0.0722;
  double K_gy = 1 - K_ry - K_by;
  double R = c.Y + c.Cr;
  double G = c.Y - (K_by / K_gy) * c.Cb - (K_ry / K_gy) * c.Cr;
  double B = c.Y + c.Cb;
  return CColorPixelDbl{Clamp(R), Clamp(G), Clamp(B)};
}
