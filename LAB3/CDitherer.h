//
// Created by @mikhirurg on 25.04.2020.
//

#ifndef COMPUTERGEOMETRY_GRAPHICS_LAB3_CDITHERER_H_
#define COMPUTERGEOMETRY_GRAPHICS_LAB3_CDITHERER_H_

#include <random>
#include "CImage.h"

struct SampleBayer {
  double *data;
  int n;

  void operator+=(double a) {
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        data[i * n + j] += a;
      }
    }
  }

  void operator/=(double a) {
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        data[i * n + j] /= a;
      }
    }
  }
};

struct ErrorDiffMatrix {
  int *data;
  int w, h;
  int x0, y0;
  int del;
  int get(int x, int y) {
    return data[y * w + x];
  }
};

template<class T>
class CDitherer {
 public:

  CDitherer(CImage<T> &img);
  ~CDitherer();
  void DoColorBitCorrection(int n);
  void DoOrderedDithering(SampleBayer bayer, int n);
  void DoRandomDithering(int n, int seed);
  void DoFloydSteinbergDithering(int n);
  void DoJJNDithering(int n);
  void DoSierraDithering(int n);
  void DoAtkinsonDithering(int n);
  void DoHalftoneDithering(int n);
  void DoErrorDiffDithering(ErrorDiffMatrix matrix, int n);

  SampleBayer SAMPLE_BAYER2;
  SampleBayer SAMPLE_BAYER4;
  SampleBayer SAMPLE_BAYER8;
  SampleBayer HALFTONE_ORTHOGONAL;

  ErrorDiffMatrix FLOYD_STEINBERG;
  ErrorDiffMatrix JJN;
  ErrorDiffMatrix ATKINSON;
  ErrorDiffMatrix SIERRA;
 private:
  CImage<T> &img_;

 private:
  SampleBayer GetBayerMatrix(int log);
  void GenBayerMatrix(int *matrix, int log, int x, int y, int n, int mult, int add);
  void RotateMatrixLeft(int *matrix, int n);
  void RotateMatrixRight(int *matrix, int n);
  void FlipMatrixByHorizontal(int *matrix, int n);
  void FlipMatrixByVertical(int *matrix, int n);
  void TransponeMatrixByMain(int *matrix, int n);
  void TransponeMatrixBySide(int *matrix, int n);
  void ApplyErrorDiffMatrix(ErrorDiffMatrix matrix, int x, int y, int err);
  void ApplyErrorDiffMatrix(ErrorDiffMatrix matrix, int x, int y, int err_r, int err_g, int err_b);
  uchar FindNearestPaletteColor(int color_val, int n);
  CMonoPixel ModifyPixelByMap(CMonoPixel pixel, int x, int y, SampleBayer bayer, int n);
  CColorPixel ModifyPixelByMap(CColorPixel pixel, int x, int y, SampleBayer bayer, int n);
  CMonoPixel ModifyPixelByRandom(CMonoPixel pixel, int n, int seed);
  CColorPixel ModifyPixelByRandom(CColorPixel pixel, int n, int seed);
  std::mt19937 rand;
};

template<class T>
CDitherer<T>::CDitherer(CImage<T> &img)
    : img_(img) {

  SAMPLE_BAYER2 = {
      new double[4]{0, 2,
                    3, 1}, (int) 2
  };

  SAMPLE_BAYER4 = {
      new double[16]{0, 8, 2, 10,
                     12, 4, 14, 6,
                     3, 11, 1, 9,
                     15, 7, 13, 5}, (int) 4
  };

  SAMPLE_BAYER8 = {
      new double[64]{0, 48, 12, 60, 3, 51, 15, 63,
                     32, 16, 44, 28, 35, 19, 47, 31,
                     8, 56, 4, 52, 11, 59, 7, 55,
                     40, 24, 36, 20, 43, 27, 39, 23,
                     2, 50, 14, 62, 1, 49, 13, 61,
                     34, 18, 46, 30, 33, 17, 45, 29,
                     10, 58, 6, 54, 9, 57, 5, 53,
                     42, 26, 38, 22, 41, 25, 37, 21}, (int) 8
  };

  HALFTONE_ORTHOGONAL = {
      new double[16]{0, 0, 0, 1,
                     0, 0, 1, 0,
                     1, 0, 0, 0,
                     0, 1, 0, 0}, (int) 4
  };
  SAMPLE_BAYER2 /= 4.0;
  SAMPLE_BAYER4 /= 16.0;
  SAMPLE_BAYER8 /= 64.0;

  FLOYD_STEINBERG = {
      new int[6]{-1, 0, 7,
                 3, 5, 1},
      (int) 3, (int) 2, (int) 1, (int) 0, (int) 16
  };

  JJN = {
      new int[15]{-1, -1, 0, 7, 5,
                  3, 4, 7, 5, 3,
                  1, 3, 5, 3, 1},
      (int) 5, (int) 3, (int) 2, (int) 0, (int) 48
  };

  ATKINSON = {
      new int[12]{-1, 0, 1, 1,
                  1, 1, 1, -1,
                  -1, 1, -1, -1},
      (int) 4, (int) 3, (int) 1, (int) 0, (int) 8
  };

  SIERRA = {
      new int[15]{-1, -1, 0, 5, 3,
                  2, 4, 5, 4, 2,
                  -1, 2, 3, 2, -1},
      (int) 5, (int) 3, (int) 2, (int) 0, (int) 32
  };

}
template<class T>
CDitherer<T>::~CDitherer() {
  delete SAMPLE_BAYER2.data;
  delete SAMPLE_BAYER4.data;
  delete SAMPLE_BAYER8.data;
  delete FLOYD_STEINBERG.data;
  delete JJN.data;
  delete ATKINSON.data;
  delete SIERRA.data;
}

template<>
void CDitherer<CMonoPixel>::ApplyErrorDiffMatrix(ErrorDiffMatrix matrix, int x, int y, int err) {
  for (int i = 0; i < matrix.h; i++) {
    for (int j = 0; j < matrix.w; j++) {
      int x1 = x + j - matrix.x0;
      int y1 = y + i - matrix.y0;
      if (matrix.get(j, i) > 0 && x1 >= 0 && x1 < img_.GetWidth() && y1 >= 0 && y1 < img_.GetHeight()) {
        img_.PutPixel(x1,
                      y1,
                      {(uchar) (img_.GetPixel(x1, y1).val + (err * matrix.get(j, i)) / matrix.del)});
      }
    }
  }
}

template<>
void CDitherer<CColorPixel>::ApplyErrorDiffMatrix(ErrorDiffMatrix matrix,
                                                  int x,
                                                  int y,
                                                  int err_r,
                                                  int err_g,
                                                  int err_b) {

  for (int i = 0; i < matrix.h; i++) {
    for (int j = 0; j < matrix.w; j++) {
      int x1 = x + j - matrix.x0;
      int y1 = y + i - matrix.y0;
      if (matrix.get(j, i) > 0 && x1 >= 0 && x1 < img_.GetWidth() && y1 >= 0 && y1 < img_.GetHeight()) {
        img_.PutPixel(x1,
                      y1,
                      {(uchar) (img_.GetPixel(x1, y1).r + (err_r * matrix.get(j, i)) / matrix.del),
                       (uchar) (img_.GetPixel(x1, y1).g + (err_g * matrix.get(j, i)) / matrix.del),
                       (uchar) (img_.GetPixel(x1, y1).b + (err_b * matrix.get(j, i)) / matrix.del)});
      }
    }
  }
}

template<>
void CDitherer<CMonoPixel>::DoColorBitCorrection(int n) {
  for (int y = 0; y < img_.GetHeight(); y++) {
    for (int x = 0; x < img_.GetWidth(); x++) {
      img_.PutPixel(x, y, {(uchar) FindNearestPaletteColor(img_.GetPixel(x, y).val, n)});
    }
  }
}

template<>
void CDitherer<CColorPixel>::DoColorBitCorrection(int n) {
  for (int y = 0; y < img_.GetHeight(); y++) {
    for (int x = 0; x < img_.GetWidth(); x++) {
      img_.PutPixel(x, y, {(uchar) FindNearestPaletteColor(img_.GetPixel(x, y).r, n),
                           (uchar) FindNearestPaletteColor(img_.GetPixel(x, y).g, n),
                           (uchar) FindNearestPaletteColor(img_.GetPixel(x, y).b, n)});
    }
  }
}

template<class T>
void CDitherer<T>::DoOrderedDithering(SampleBayer bayer, int n) {
  for (int y = 0; y < img_.GetHeight(); y++) {
    for (int x = 0; x < img_.GetWidth(); x++) {
      img_.PutPixel(x, y, {ModifyPixelByMap(img_.GetPixel(x, y), x, y, bayer, n)});
    }
  }
}

template<class T>
void CDitherer<T>::DoRandomDithering(int n, int seed) {
  rand.seed(seed);
  for (int y = 0; y < img_.GetHeight(); y++) {
    for (int x = 0; x < img_.GetWidth(); x++) {
      img_.PutPixel(x, y, {ModifyPixelByRandom(img_.GetPixel(x, y), n, seed)});
    }
  }
}

template<>
void CDitherer<CMonoPixel>::DoFloydSteinbergDithering(int n) {
  for (int y = 0; y < img_.GetHeight(); y++) {
    for (int x = 0; x < img_.GetWidth(); x++) {
      CMonoPixel old_pixel = img_.GetPixel(x, y);
      CMonoPixel new_pixel = {FindNearestPaletteColor(old_pixel.val, n)};
      img_.PutPixel(x, y, new_pixel);
      int quant_err = old_pixel.val - new_pixel.val;
      if (x + 1 < img_.GetWidth()) {
        img_.PutPixel(x + 1, y, {(uchar) (img_.GetPixel(x + 1, y).val + quant_err * 7.0 / 16.0)});
      }
      if (x - 1 >= 0 && y + 1 < img_.GetHeight()) {
        img_.PutPixel(x - 1, y + 1, {(uchar) (img_.GetPixel(x - 1, y + 1).val + quant_err * 3.0 / 16.0)});
      }
      if (y + 1 < img_.GetHeight()) {
        img_.PutPixel(x, y + 1, {(uchar) (img_.GetPixel(x, y + 1).val + quant_err * 5.0 / 16.0)});
      }
      if (y + 1 < img_.GetHeight() && x + 1 < img_.GetWidth()) {
        img_.PutPixel(x + 1, y + 1, {(uchar) (img_.GetPixel(x + 1, y + 1).val + quant_err * 1.0 / 16.0)});
      }
    }
  }
}

template<>
void CDitherer<CColorPixel>::DoFloydSteinbergDithering(int n) {
  for (int y = 0; y < img_.GetHeight(); y++) {
    for (int x = 0; x < img_.GetWidth(); x++) {
      CColorPixel old_pixel = img_.GetPixel(x, y);
      CColorPixel new_pixel = {FindNearestPaletteColor(old_pixel.r, n),
                               FindNearestPaletteColor(old_pixel.g, n),
                               FindNearestPaletteColor(old_pixel.b, n)};
      img_.PutPixel(x, y, new_pixel);
      int quant_err_r = old_pixel.r - new_pixel.r;
      int quant_err_g = old_pixel.g - new_pixel.g;
      int quant_err_b = old_pixel.b - new_pixel.b;
      if (x + 1 < img_.GetWidth()) {
        img_.PutPixel(x + 1, y, {(uchar) (img_.GetPixel(x + 1, y).r + (quant_err_r * 7.0 / 16.0)),
                                 (uchar) (img_.GetPixel(x + 1, y).g + (quant_err_g * 7.0 / 16.0)),
                                 (uchar) (img_.GetPixel(x + 1, y).b + (quant_err_b * 7.0 / 16.0))});
      }
      if (x - 1 >= 0 && y + 1 < img_.GetHeight()) {
        img_.PutPixel(x - 1, y + 1, {(uchar) (img_.GetPixel(x - 1, y + 1).r + (quant_err_r * 3.0 / 16.0)),
                                     (uchar) (img_.GetPixel(x - 1, y + 1).g + (quant_err_g * 3.0 / 16.0)),
                                     (uchar) (img_.GetPixel(x - 1, y + 1).b + (quant_err_b * 3.0 / 16.0))});
      }
      if (y + 1 < img_.GetHeight()) {
        img_.PutPixel(x, y + 1, {(uchar) (img_.GetPixel(x, y + 1).r + (quant_err_r * 5.0 / 16.0)),
                                 (uchar) (img_.GetPixel(x, y + 1).g + (quant_err_g * 5.0 / 16.0)),
                                 (uchar) (img_.GetPixel(x, y + 1).b + (quant_err_b * 5.0 / 16.0))});
      }
      if (y + 1 < img_.GetHeight() && x + 1 < img_.GetWidth()) {
        img_.PutPixel(x + 1, y + 1, {(uchar) (img_.GetPixel(x + 1, y + 1).r + ((quant_err_r * 1.0) / 16)),
                                     (uchar) (img_.GetPixel(x + 1, y + 1).g + (quant_err_g * 1.0 / 16)),
                                     (uchar) (img_.GetPixel(x + 1, y + 1).b + (quant_err_b * 1.0 / 16))});
      }
    }
  }
}

template<>
void CDitherer<CColorPixel>::DoJJNDithering(int n) {
  for (int y = 0; y < img_.GetHeight(); y++) {
    for (int x = 0; x < img_.GetWidth(); x++) {
      CColorPixel old_pixel = img_.GetPixel(x, y);
      CColorPixel new_pixel = {FindNearestPaletteColor(old_pixel.r, n),
                               FindNearestPaletteColor(old_pixel.g, n),
                               FindNearestPaletteColor(old_pixel.b, n)};
      img_.PutPixel(x, y, new_pixel);
      int quant_err_r = old_pixel.r - new_pixel.r;
      int quant_err_g = old_pixel.g - new_pixel.g;
      int quant_err_b = old_pixel.b - new_pixel.b;

      double del = 48.0;

      if (x + 1 < img_.GetWidth()) {
        img_.PutPixel(x + 1, y, {(uchar) (img_.GetPixel(x + 1, y).r + (quant_err_r * 7.0 / del)),
                                 (uchar) (img_.GetPixel(x + 1, y).g + (quant_err_g * 7.0 / del)),
                                 (uchar) (img_.GetPixel(x + 1, y).b + (quant_err_b * 7.0 / del))});
      }
      if (x + 2 < img_.GetWidth()) {
        img_.PutPixel(x + 2, y, {(uchar) (img_.GetPixel(x + 2, y).r + (quant_err_r * 5.0 / del)),
                                 (uchar) (img_.GetPixel(x + 2, y).g + (quant_err_g * 5.0 / del)),
                                 (uchar) (img_.GetPixel(x + 2, y).b + (quant_err_b * 5.0 / del))});
      }
      if ((x - 2 >= 0) && (y + 1 < img_.GetHeight())) {
        img_.PutPixel(x - 2, y + 1, {(uchar) (img_.GetPixel(x - 2, y + 1).r + (quant_err_r * 3.0 / del)),
                                     (uchar) (img_.GetPixel(x - 2, y + 1).g + (quant_err_g * 3.0 / del)),
                                     (uchar) (img_.GetPixel(x - 2, y + 1).b + (quant_err_b * 3.0 / del))});
      }
      if ((x - 1 >= 0) && (y + 1 < img_.GetHeight())) {
        img_.PutPixel(x - 1, y + 1, {(uchar) (img_.GetPixel(x - 1, y + 1).r + (quant_err_r * 5.0 / del)),
                                     (uchar) (img_.GetPixel(x - 1, y + 1).g + (quant_err_g * 5.0 / del)),
                                     (uchar) (img_.GetPixel(x - 1, y + 1).b + (quant_err_b * 5.0 / del))});
      }
      if (y + 1 < img_.GetHeight()) {
        img_.PutPixel(x, y + 1, {(uchar) (img_.GetPixel(x, y + 1).r + (quant_err_r * 7.0 / del)),
                                 (uchar) (img_.GetPixel(x, y + 1).g + (quant_err_g * 7.0 / del)),
                                 (uchar) (img_.GetPixel(x, y + 1).b + (quant_err_b * 7.0 / del))});
      }

      if ((x + 1 < img_.GetWidth()) && (y + 1 < img_.GetHeight())) {
        img_.PutPixel(x + 1, y + 1, {(uchar) (img_.GetPixel(x + 1, y + 1).r + (quant_err_r * 5.0 / del)),
                                     (uchar) (img_.GetPixel(x + 1, y + 1).g + (quant_err_g * 5.0 / del)),
                                     (uchar) (img_.GetPixel(x + 1, y + 1).b + (quant_err_b * 5.0 / del))});
      }

      if ((x + 2 < img_.GetWidth()) && (y + 1 < img_.GetHeight())) {
        img_.PutPixel(x + 2, y + 1, {(uchar) (img_.GetPixel(x + 2, y + 1).r + (quant_err_r * 3.0 / del)),
                                     (uchar) (img_.GetPixel(x + 2, y + 1).g + (quant_err_g * 3.0 / del)),
                                     (uchar) (img_.GetPixel(x + 2, y + 1).b + (quant_err_b * 3.0 / del))});
      }
      if ((x - 2 >= 0) && (y + 2 < img_.GetHeight())) {
        img_.PutPixel(x - 2, y + 2, {(uchar) (img_.GetPixel(x - 2, y + 2).r + ((quant_err_r) / del)),
                                     (uchar) (img_.GetPixel(x - 2, y + 2).g + ((quant_err_g) / del)),
                                     (uchar) (img_.GetPixel(x - 2, y + 2).b + ((quant_err_b) / del))});
      }
      if ((x - 1 >= 0) && (y + 2 < img_.GetHeight())) {
        img_.PutPixel(x - 1, y + 2, {(uchar) (img_.GetPixel(x - 1, y + 2).r + (quant_err_r * 3.0 / del)),
                                     (uchar) (img_.GetPixel(x - 1, y + 2).g + (quant_err_g * 3.0 / del)),
                                     (uchar) (img_.GetPixel(x - 1, y + 2).b + (quant_err_b * 3.0 / del))});
      }
      if (y + 2 < img_.GetHeight()) {
        img_.PutPixel(x, y + 2, {(uchar) (img_.GetPixel(x, y + 2).r + (quant_err_r * 5.0 / del)),
                                 (uchar) (img_.GetPixel(x, y + 2).g + (quant_err_g * 5.0 / del)),
                                 (uchar) (img_.GetPixel(x, y + 2).b + (quant_err_b * 5.0 / del))});
      }
      if ((x + 1 < img_.GetWidth()) && (y + 2 < img_.GetHeight())) {
        img_.PutPixel(x + 1, y + 2, {(uchar) (img_.GetPixel(x + 1, y + 2).r + (quant_err_r * 3.0 / del)),
                                     (uchar) (img_.GetPixel(x + 1, y + 2).g + (quant_err_g * 3.0 / del)),
                                     (uchar) (img_.GetPixel(x + 1, y + 2).b + (quant_err_b * 3.0 / del))});
      }
      if ((x + 2 < img_.GetWidth()) && (y + 2 < img_.GetHeight())) {
        img_.PutPixel(x + 2, y + 2, {(uchar) (img_.GetPixel(x + 2, y + 2).r + (quant_err_r / del)),
                                     (uchar) (img_.GetPixel(x + 2, y + 2).g + (quant_err_g / del)),
                                     (uchar) (img_.GetPixel(x + 2, y + 2).b + (quant_err_b / del))});
      }
    }
  }
}

template<>
void CDitherer<CMonoPixel>::DoJJNDithering(int n) {
  for (int y = 0; y < img_.GetHeight(); y++) {
    for (int x = 0; x < img_.GetWidth(); x++) {
      CMonoPixel old_pixel = img_.GetPixel(x, y);
      CMonoPixel new_pixel = {FindNearestPaletteColor(old_pixel.val, n)};
      img_.PutPixel(x, y, new_pixel);
      int quant_err = old_pixel.val - new_pixel.val;
      if (x + 1 < img_.GetWidth()) {
        img_.PutPixel(x + 1, y, {(uchar) (img_.GetPixel(x + 1, y).val + ((quant_err * 7) / 48))});
      }
      if (x + 2 < img_.GetWidth()) {
        img_.PutPixel(x + 2, y, {(uchar) (img_.GetPixel(x + 2, y).val + ((quant_err * 5) / 48))});
      }
      if (x - 2 >= 0 && y + 1 < img_.GetHeight()) {
        img_.PutPixel(x - 2, y + 1, {(uchar) (img_.GetPixel(x - 2, y + 1).val + ((quant_err * 3) / 48))});
      }
      if (x - 1 >= 0 && y + 1 < img_.GetHeight()) {
        img_.PutPixel(x - 1, y + 1, {(uchar) (img_.GetPixel(x - 1, y + 1).val + ((quant_err * 5) / 48))});
      }
      if (y + 1 < img_.GetHeight()) {
        img_.PutPixel(x, y + 1, {(uchar) (img_.GetPixel(x, y + 1).val + ((quant_err * 7) / 48))});
      }

      if (x + 1 < img_.GetWidth() && y + 1 < img_.GetHeight()) {
        img_.PutPixel(x + 1, y + 1, {(uchar) (img_.GetPixel(x + 1, y + 1).val + ((quant_err * 5) / 48))});
      }

      if (x + 2 < img_.GetWidth() && y + 1 < img_.GetHeight()) {
        img_.PutPixel(x + 2, y + 1, {(uchar) (img_.GetPixel(x + 2, y + 1).val + ((quant_err * 3) / 48))});
      }
      if (x - 2 >= 0 && y + 2 < img_.GetHeight()) {
        img_.PutPixel(x - 2, y + 2, {(uchar) (img_.GetPixel(x - 2, y + 2).val + ((quant_err) / 48))});
      }
      if (x - 1 >= 0 && y + 2 < img_.GetHeight()) {
        img_.PutPixel(x - 1, y + 2, {(uchar) (img_.GetPixel(x - 1, y + 2).val + ((quant_err * 3) / 48))});
      }
      if (y + 2 < img_.GetHeight()) {
        img_.PutPixel(x, y + 2, {(uchar) (img_.GetPixel(x, y + 2).val + ((quant_err * 5) / 48))});
      }
      if (x + 1 < img_.GetWidth() && y + 2 < img_.GetHeight()) {
        img_.PutPixel(x + 1, y + 2, {(uchar) (img_.GetPixel(x + 1, y + 2).val + ((quant_err * 3.0) / 48))});
      }
      if (x + 2 < img_.GetWidth() && y + 2 < img_.GetHeight()) {
        img_.PutPixel(x + 2, y + 2, {(uchar) (img_.GetPixel(x + 2, y + 2).val + ((quant_err) / 48))});
      }
    }
  }
}

template<>
void CDitherer<CMonoPixel>::DoAtkinsonDithering(int n) {
  for (int y = 0; y < img_.GetHeight(); y++) {
    for (int x = 0; x < img_.GetWidth(); x++) {
      CMonoPixel old_pixel = img_.GetPixel(x, y);
      CMonoPixel new_pixel = {FindNearestPaletteColor(old_pixel.val, n)};
      int quant_err = old_pixel.val - new_pixel.val;
      img_.PutPixel(x, y, new_pixel);
      if (x + 1 < img_.GetWidth()) {
        img_.PutPixel(x + 1, y, {(uchar) (img_.GetPixel(x + 1, y).val + ((quant_err) / 8))});
      }
      if (x + 2 < img_.GetWidth()) {
        img_.PutPixel(x + 2, y, {(uchar) (img_.GetPixel(x + 2, y).val + ((quant_err) / 8))});
      }
      if (x - 1 >= 0 && y + 1 < img_.GetHeight()) {
        img_.PutPixel(x - 1, y + 1, {(uchar) (img_.GetPixel(x - 1, y + 1).val + ((quant_err) / 8))});
      }
      if (y + 1 < img_.GetHeight()) {
        img_.PutPixel(x, y + 1, {(uchar) (img_.GetPixel(x, y + 1).val + ((quant_err) / 8))});
      }
      if (x + 1 < img_.GetWidth() && y + 1 < img_.GetHeight()) {
        img_.PutPixel(x + 1, y + 1, {(uchar) (img_.GetPixel(x + 1, y + 1).val + ((quant_err) / 8))});
      }
      if (y + 2 < img_.GetHeight()) {
        img_.PutPixel(x, y + 2, {(uchar) (img_.GetPixel(x, y + 2).val + ((quant_err) / 8))});
      }
    }
  }
}

template<>
void CDitherer<CColorPixel>::DoAtkinsonDithering(int n) {
  for (int y = 0; y < img_.GetHeight(); y++) {
    for (int x = 0; x < img_.GetWidth(); x++) {
      CColorPixel old_pixel = img_.GetPixel(x, y);
      CColorPixel new_pixel = {FindNearestPaletteColor(old_pixel.r, n),
                               FindNearestPaletteColor(old_pixel.g, n),
                               FindNearestPaletteColor(old_pixel.b, n)};

      int quant_err_r = old_pixel.r - new_pixel.r;
      int quant_err_g = old_pixel.g - new_pixel.g;
      int quant_err_b = old_pixel.b - new_pixel.b;

      img_.PutPixel(x, y, new_pixel);
      if (x + 1 < img_.GetWidth()) {
        img_.PutPixel(x + 1, y, {(uchar) (img_.GetPixel(x + 1, y).r + ((quant_err_r) / 8)),
                                 (uchar) (img_.GetPixel(x + 1, y).g + ((quant_err_g) / 8)),
                                 (uchar) (img_.GetPixel(x + 1, y).b + ((quant_err_b) / 8))});
      }
      if (x + 2 < img_.GetWidth()) {
        img_.PutPixel(x + 2, y, {(uchar) (img_.GetPixel(x + 2, y).r + ((quant_err_r) / 8)),
                                 (uchar) (img_.GetPixel(x + 2, y).g + ((quant_err_g) / 8)),
                                 (uchar) (img_.GetPixel(x + 2, y).b + ((quant_err_b) / 8))});
      }
      if (x - 1 >= 0 && y + 1 < img_.GetHeight()) {
        img_.PutPixel(x - 1, y + 1, {(uchar) (img_.GetPixel(x - 1, y + 1).r + ((quant_err_r) / 8)),
                                     (uchar) (img_.GetPixel(x - 1, y + 1).g + ((quant_err_g) / 8)),
                                     (uchar) (img_.GetPixel(x - 1, y + 1).b + ((quant_err_b) / 8))});
      }
      if (y + 1 < img_.GetHeight()) {
        img_.PutPixel(x, y + 1, {(uchar) (img_.GetPixel(x, y + 1).r + ((quant_err_r) / 8)),
                                 (uchar) (img_.GetPixel(x, y + 1).g + ((quant_err_g) / 8)),
                                 (uchar) (img_.GetPixel(x, y + 1).b + ((quant_err_b) / 8))});
      }
      if (x + 1 < img_.GetWidth() && y + 1 < img_.GetHeight()) {
        img_.PutPixel(x + 1, y + 1, {(uchar) (img_.GetPixel(x + 1, y + 1).r + ((quant_err_r) / 8)),
                                     (uchar) (img_.GetPixel(x + 1, y + 1).g + ((quant_err_g) / 8)),
                                     (uchar) (img_.GetPixel(x + 1, y + 1).b + ((quant_err_b) / 8))});
      }
      if (y + 2 < img_.GetHeight()) {
        img_.PutPixel(x, y + 2, {(uchar) (img_.GetPixel(x, y + 2).r + ((quant_err_r) / 8)),
                                 (uchar) (img_.GetPixel(x, y + 2).g + ((quant_err_g) / 8)),
                                 (uchar) (img_.GetPixel(x, y + 2).b + ((quant_err_b) / 8))});
      }
    }
  }
}

template<>
void CDitherer<CMonoPixel>::DoSierraDithering(int n) {
  for (int y = 0; y < img_.GetHeight(); y++) {
    for (int x = 0; x < img_.GetWidth(); x++) {
      CMonoPixel old_pixel = img_.GetPixel(x, y);
      CMonoPixel new_pixel = {FindNearestPaletteColor(old_pixel.val, n)};
      img_.PutPixel(x, y, new_pixel);
      int quant_err = old_pixel.val - new_pixel.val;
      if (x + 1 < img_.GetWidth()) {
        img_.PutPixel(x + 1, y, {(uchar) (img_.GetPixel(x + 1, y).val + ((quant_err * 5) / 32))});
      }
      if (x + 2 < img_.GetWidth()) {
        img_.PutPixel(x + 2, y, {(uchar) (img_.GetPixel(x + 2, y).val + ((quant_err * 3) / 32))});
      }
      if (x - 2 >= 0 && y + 1 < img_.GetHeight()) {
        img_.PutPixel(x - 2, y + 1, {(uchar) (img_.GetPixel(x - 2, y + 1).val + ((quant_err * 2) / 32))});
      }
      if (x - 1 >= 0 && y + 1 < img_.GetHeight()) {
        img_.PutPixel(x - 1, y + 1, {(uchar) (img_.GetPixel(x - 1, y + 1).val + ((quant_err * 4) / 32))});
      }
      if (y + 1 < img_.GetHeight()) {
        img_.PutPixel(x, y + 1, {(uchar) (img_.GetPixel(x, y + 1).val + ((quant_err * 5) / 32))});
      }

      if (x + 1 < img_.GetWidth() && y + 1 < img_.GetHeight()) {
        img_.PutPixel(x + 1, y + 1, {(uchar) (img_.GetPixel(x + 1, y + 1).val + ((quant_err * 4) / 32))});
      }

      if (x + 2 < img_.GetWidth() && y + 1 < img_.GetHeight()) {
        img_.PutPixel(x + 2, y + 1, {(uchar) (img_.GetPixel(x + 2, y + 1).val + ((quant_err * 2) / 32))});
      }
      if (x - 1 >= 0 && y + 2 < img_.GetHeight()) {
        img_.PutPixel(x - 1, y + 2, {(uchar) (img_.GetPixel(x - 1, y + 2).val + ((quant_err * 2) / 32))});
      }
      if (y + 2 < img_.GetHeight()) {
        img_.PutPixel(x, y + 2, {(uchar) (img_.GetPixel(x, y + 2).val + ((quant_err * 3) / 32))});
      }
      if (x + 1 < img_.GetWidth() && y + 2 < img_.GetHeight()) {
        img_.PutPixel(x + 1, y + 2, {(uchar) (img_.GetPixel(x + 1, y + 2).val + ((quant_err * 2) / 32))});
      }
    }
  }
}

template<>
void CDitherer<CColorPixel>::DoSierraDithering(int n) {
  for (int y = 0; y < img_.GetHeight(); y++) {
    for (int x = 0; x < img_.GetWidth(); x++) {
      CColorPixel old_pixel = img_.GetPixel(x, y);
      CColorPixel new_pixel = {FindNearestPaletteColor(old_pixel.r, n),
                               FindNearestPaletteColor(old_pixel.g, n),
                               FindNearestPaletteColor(old_pixel.b, n)};
      img_.PutPixel(x, y, new_pixel);
      int quant_err_r = old_pixel.r - new_pixel.r;
      int quant_err_g = old_pixel.g - new_pixel.g;
      int quant_err_b = old_pixel.b - new_pixel.b;

      if (x + 1 < img_.GetWidth()) {
        img_.PutPixel(x + 1, y, {(uchar) (img_.GetPixel(x + 1, y).r + ((quant_err_r * 5) / 32)),
                                 (uchar) (img_.GetPixel(x + 1, y).g + ((quant_err_g * 5) / 32)),
                                 (uchar) (img_.GetPixel(x + 1, y).b + ((quant_err_b * 5) / 32))});
      }
      if (x + 2 < img_.GetWidth()) {
        img_.PutPixel(x + 2, y, {(uchar) (img_.GetPixel(x + 2, y).r + ((quant_err_r * 3) / 32)),
                                 (uchar) (img_.GetPixel(x + 2, y).g + ((quant_err_g * 3) / 32)),
                                 (uchar) (img_.GetPixel(x + 2, y).b + ((quant_err_b * 3) / 32))});
      }
      if (x - 2 >= 0 && y + 1 < img_.GetHeight()) {
        img_.PutPixel(x - 2, y + 1, {(uchar) (img_.GetPixel(x - 2, y + 1).r + ((quant_err_r * 2) / 32)),
                                     (uchar) (img_.GetPixel(x - 2, y + 1).g + ((quant_err_g * 2) / 32)),
                                     (uchar) (img_.GetPixel(x - 2, y + 1).b + ((quant_err_b * 2) / 32))});
      }
      if (x - 1 >= 0 && y + 1 < img_.GetHeight()) {
        img_.PutPixel(x - 1, y + 1, {(uchar) (img_.GetPixel(x - 1, y + 1).r + ((quant_err_r * 4) / 32)),
                                     (uchar) (img_.GetPixel(x - 1, y + 1).g + ((quant_err_g * 4) / 32)),
                                     (uchar) (img_.GetPixel(x - 1, y + 1).b + ((quant_err_b * 4) / 32))});
      }
      if (y + 1 < img_.GetHeight()) {
        img_.PutPixel(x, y + 1, {(uchar) (img_.GetPixel(x, y + 1).r + ((quant_err_r * 5) / 32)),
                                 (uchar) (img_.GetPixel(x, y + 1).g + ((quant_err_g * 5) / 32)),
                                 (uchar) (img_.GetPixel(x, y + 1).b + ((quant_err_b * 5) / 32))});
      }

      if (x + 1 < img_.GetWidth() && y + 1 < img_.GetHeight()) {
        img_.PutPixel(x + 1, y + 1, {(uchar) (img_.GetPixel(x + 1, y + 1).r + ((quant_err_r * 4) / 32)),
                                     (uchar) (img_.GetPixel(x + 1, y + 1).g + ((quant_err_g * 4) / 32)),
                                     (uchar) (img_.GetPixel(x + 1, y + 1).b + ((quant_err_b * 4) / 32))});
      }

      if (x + 2 < img_.GetWidth() && y + 1 < img_.GetHeight()) {
        img_.PutPixel(x + 2, y + 1, {(uchar) (img_.GetPixel(x + 2, y + 1).r + ((quant_err_r * 2) / 32)),
                                     (uchar) (img_.GetPixel(x + 2, y + 1).g + ((quant_err_g * 2) / 32)),
                                     (uchar) (img_.GetPixel(x + 2, y + 1).b + ((quant_err_b * 2) / 32))});
      }
      if (x - 1 >= 0 && y + 2 < img_.GetHeight()) {
        img_.PutPixel(x - 1, y + 2, {(uchar) (img_.GetPixel(x - 1, y + 2).r + ((quant_err_r * 2) / 32)),
                                     (uchar) (img_.GetPixel(x - 1, y + 2).g + ((quant_err_g * 2) / 32)),
                                     (uchar) (img_.GetPixel(x - 1, y + 2).b + ((quant_err_b * 2) / 32))});
      }
      if (y + 2 < img_.GetHeight()) {
        img_.PutPixel(x, y + 2, {(uchar) (img_.GetPixel(x, y + 2).r + ((quant_err_r * 3) / 32)),
                                 (uchar) (img_.GetPixel(x, y + 2).g + ((quant_err_g * 3) / 32)),
                                 (uchar) (img_.GetPixel(x, y + 2).b + ((quant_err_b * 3) / 32))});
      }
      if (x + 1 < img_.GetWidth() && y + 2 < img_.GetHeight()) {
        img_.PutPixel(x + 1, y + 2, {(uchar) (img_.GetPixel(x + 1, y + 2).r + ((quant_err_r * 2) / 32)),
                                     (uchar) (img_.GetPixel(x + 1, y + 2).g + ((quant_err_g * 2) / 32)),
                                     (uchar) (img_.GetPixel(x + 1, y + 2).b + ((quant_err_b * 2) / 32))});
      }
    }
  }
}

template<class T>
void CDitherer<T>::DoHalftoneDithering(int n) {
  for (int y = 0; y < img_.GetHeight(); y++) {
    for (int x = 0; x < img_.GetWidth(); x++) {
      img_.PutPixel(x, y, {ModifyPixelByMap(img_.GetPixel(x, y), x, y, HALFTONE_ORTHOGONAL, n)});
    }
  }
}

template<>
void CDitherer<CMonoPixel>::DoErrorDiffDithering(ErrorDiffMatrix matrix, int n) {
  for (int y = 0; y < img_.GetHeight(); y++) {
    for (int x = 0; x < img_.GetWidth(); x++) {
      CMonoPixel old_pixel = img_.GetPixel(x, y);
      CMonoPixel new_pixel = {FindNearestPaletteColor(old_pixel.val, n)};
      img_.PutPixel(x, y, new_pixel);
      int quant_err = old_pixel.val - new_pixel.val;
      ApplyErrorDiffMatrix(matrix, x, y, quant_err);
    }
  }
}

template<>
void CDitherer<CColorPixel>::DoErrorDiffDithering(ErrorDiffMatrix matrix, int n) {
  for (int y = 0; y < img_.GetHeight(); y++) {
    for (int x = 0; x < img_.GetWidth(); x++) {
      CColorPixel old_pixel = img_.GetPixel(x, y);
      CColorPixel new_pixel = {FindNearestPaletteColor(old_pixel.r, n),
                               FindNearestPaletteColor(old_pixel.g, n),
                               FindNearestPaletteColor(old_pixel.b, n)};
      img_.PutPixel(x, y, new_pixel);
      int quant_err_r = old_pixel.r - new_pixel.r;
      int quant_err_g = old_pixel.g - new_pixel.g;
      int quant_err_b = old_pixel.b - new_pixel.b;

      ApplyErrorDiffMatrix(matrix, x, y, quant_err_r, quant_err_g, quant_err_b);
    }
  }
}

template<class T>
uchar CDitherer<T>::FindNearestPaletteColor(int color_val, int n) {
  color_val = std::max(color_val, 0);
  color_val = std::min(color_val, 255);
  int out = 0;
  int tmp = (color_val >> (8 - n)) << (8 - n);
  for (int i = 0; i < 8 / n; i++) {
    out += tmp;
    tmp = tmp >> n;
  }
  return (uchar) out;
}

template<>
CMonoPixel CDitherer<CMonoPixel>::ModifyPixelByMap(CMonoPixel pixel, int x, int y, SampleBayer bayer, int n) {
  double resizer = 255.0 / bayer.n;
  return {FindNearestPaletteColor(pixel.val + int(resizer * bayer.data[(y % bayer.n) * bayer.n + x % bayer.n]),
                                  n)};
}

template<>
CColorPixel CDitherer<CColorPixel>::ModifyPixelByMap(CColorPixel pixel, int x, int y, SampleBayer bayer, int n) {
  double resizer = 255.0 / bayer.n;
  return {FindNearestPaletteColor(pixel.r + int(resizer * bayer.data[(y % bayer.n) * bayer.n + x % bayer.n]),
                                  n),
          FindNearestPaletteColor(pixel.g + int(resizer * bayer.data[(y % bayer.n) * bayer.n + x % bayer.n]),
                                  n),
          FindNearestPaletteColor(pixel.r + int(resizer * bayer.data[(y % bayer.n) * bayer.n + x % bayer.n]),
                                  n)};
}

template<>
CColorPixel CDitherer<CColorPixel>::ModifyPixelByRandom(CColorPixel pixel, int n, int seed) {
  double resizer = 255.0 / n;
  std::uniform_real_distribution<> urd(0 + DBL_EPSILON, 1 + DBL_EPSILON);
  return {FindNearestPaletteColor(pixel.r + int(resizer * urd(rand)), n),
          FindNearestPaletteColor(pixel.g + int(resizer * urd(rand)), n),
          FindNearestPaletteColor(pixel.b + int(resizer * urd(rand)), n)};
}

template<>
CMonoPixel CDitherer<CMonoPixel>::ModifyPixelByRandom(CMonoPixel color_val, int n, int seed) {
  double resizer = 255.0 / n;
  std::uniform_real_distribution<> urd(-0.5 + DBL_EPSILON, 0.5 + DBL_EPSILON);
  return {FindNearestPaletteColor(color_val.val + int(resizer * urd(rand)), n)};
}

template<class T>
void CDitherer<T>::GenBayerMatrix(int *matrix, int log, int x, int y, int n, int mult, int add) {
  int w = (int) pow(2, log) / 2;
  if (log == 1) {
    matrix[y * n + x] = add;
    matrix[y * n + x + 1] = 2 * mult + add;
    matrix[(y + 1) * n + x] = 3 * mult + add;
    matrix[(y + 1) * n + x + 1] = mult + add;
  } else {
    GenBayerMatrix(matrix, log - 1, x, y, n, mult * 4, add * mult);
    GenBayerMatrix(matrix, log - 1, x + w, y, n, mult * 4, add * mult + 2);
    GenBayerMatrix(matrix, log - 1, x, y + w, n, mult * 4, add * mult + 3);
    GenBayerMatrix(matrix, log - 1, x + w, y + w, n, mult * 4, add * mult + 1);
  }
}

template<class T>
SampleBayer CDitherer<T>::GetBayerMatrix(int log) {
  SampleBayer bayer{
      new double[(int) pow(2, log) * (int) pow(2, log)],
      (int) pow(2, log)
  };
  GenBayerMatrix(bayer.data, log, 0, 0, (int) pow(2, log), 1, 0);
  return bayer;
}

template<class T>
void CDitherer<T>::FlipMatrixByHorizontal(int *matrix, int n) {
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n / 2; j++) {
      std::swap(matrix[i * n + j], matrix[i * n + (n - j - 1)]);
    }
  }
}

template<class T>
void CDitherer<T>::FlipMatrixByVertical(int *matrix, int n) {
  for (int i = 0; i < n / 2; i++) {
    for (int j = 0; j < n; j++) {
      std::swap(matrix[i * n + j], matrix[(n - i - 1) * n + j]);
    }
  }
}

template<class T>
void CDitherer<T>::RotateMatrixLeft(int *matrix, int n) {
  auto *tmp = new int[n * n];
  int k = 0;
  for (int j = 0; j < n; j++) {
    for (int i = n - 1; i >= 0; i--) {
      tmp[k] = matrix[i * n + j];
      k++;
    }
  }
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      matrix[i * n + j] = tmp[i * n + j];
    }
  }
  delete[] (tmp);
}

template<class T>
void CDitherer<T>::RotateMatrixRight(int *matrix, int n) {
  auto *tmp = new int[n * n];
  int k = 0;
  for (int j = n - 1; j >= 0; j--) {
    for (int i = 0; i < n; i++) {
      tmp[k] = matrix[i * n + j];
      k++;
    }
  }
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      matrix[i * n + j] = tmp[i * n + j];
    }
  }
  delete[] (tmp);
}

template<class T>
void CDitherer<T>::TransponeMatrixByMain(int *matrix, int n) {
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < i; j++) {
      std::swap(matrix[i * n + j], matrix[j * n + i]);
    }
  }
}

template<class T>
void CDitherer<T>::TransponeMatrixBySide(int *matrix, int n) {
  for (int i = 0; i <= n / 2; i++) {
    for (int j = n / 2; j >= i; j--) {
      std::swap(matrix[i * n + j], matrix[j * n + i]);
    }
  }
}


#endif //COMPUTERGEOMETRY_GRAPHICS_LAB3_CDITHERER_H_
