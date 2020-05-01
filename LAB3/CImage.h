//
// Created by @mikhirurg on 11.04.2020.
//

#ifndef COMPUTERGEOMETRY_GRAPHICS_CIMAGE_H
#define COMPUTERGEOMETRY_GRAPHICS_CIMAGE_H
typedef unsigned char uchar;
#include <string>
#include <vector>
#include <cfloat>
#include <algorithm>
#include <cmath>
#include <map>
#include <set>

#include "CImage.h"
#include "CImageFileOpenException.h"
#include "CImageFileDeleteException.h"
#include "CImageParamsException.h"
#include "CImageMemAllocException.h"
#include "CImageFileFormatException.h"
#include "CImageFileReadException.h"

enum FileType {
  P5 = 5,
  P6
};

struct CMonoPixel {
  uchar val;
};

struct CColorPixel {
  uchar r, g, b;
};

template<class T>
class CImage {
 public:
  explicit CImage(const std::string &fname, double gamma);

  CImage(const std::string &fname, FileType type, int w, int h, int max_val, double gamma);

  CImage(const std::string &fname, FileType type, int w, int h, int max_val, const T *&data, double gamma);

  CImage(int w, int h, int max_val, FileType type, double gamma);

  CImage(const CImage &img);

  ~CImage();

  void WriteImg(const std::string &fname);

  void WriteImg();

  T GetPixel(int x, int y) const;

  T GetLinearPixel(int x, int y) const;

  void PutPixel(int x, int y, T pixel);

  void PutPixelWithGamma(int x, int y, T pixel);

  void CorrectImageWithGamma();

  T *operator[](int i);

  int GetWidth() const;

  int GetHeight() const;

  int GetMaxVal() const;

  FileType GetFileType() const;

  void FillWithGradient();

 private:
  const double eps = 1e-10;
  const int MAX_HEADER_SIZE = 50;
  std::string fname_;
  FileType type_;
  int w_, h_;
  int max_val_;
  T *data_;
  double gamma_;

  bool FileExists(const char *s);

  double IntPart(double x);

  double FloatPart(double x);
};

template<typename T>
CImage<T>::CImage(const std::string &fname, double gamma)
    : fname_(fname), gamma_(gamma) {
  FILE *f = fopen(fname.c_str(), "rb");
  if (!f) {
    throw CImageFileOpenException();
  }
  int i = fscanf(f, "P%i%i%i%i\n", &type_, &w_, &h_, &max_val_);
  if (i != 4 || w_ <= 0 || h_ <= 0 || max_val_ <= 0) {
    fclose(f);
    throw CImageParamsException();
  }
  if (type_ != P5 && type_ != P6) {
    fclose(f);
    throw CImageFileFormatException();
  }
  try {
    data_ = new T[w_ * h_];
    int check = fread(data_, sizeof(T), w_ * h_, f);
    if (check != w_ * h_) {
      fclose(f);
      throw CImageFileReadException();
    }
  } catch (std::bad_alloc &) {
    throw CImageMemAllocException();
  }
  fclose(f);
}

template<typename T>
CImage<T>::~CImage() {
  delete[](data_);
}

template<typename T>
CImage<T>::CImage(const std::string &fname, FileType type, int w, int h,
                  int max_val, double gamma)
    : fname_(fname), type_(type), w_(w), h_(h), max_val_(max_val), gamma_(gamma) {
  try {
    data_ = new T[w * h];
  } catch (std::bad_alloc &e) {
    throw CImageMemAllocException();
  }
}

template<typename T>
CImage<T>::CImage(const std::string &fname, FileType type, int w, int h,
                  int max_val, const T *&data, double gamma)
    : fname_(fname), type_(type), w_(w), h_(h), max_val_(max_val), gamma_(gamma) {
  try {
    data_ = new T[w_ * h_];
  } catch (std::bad_alloc &e) {
    throw CImageMemAllocException();
  }
  for (int i = 0; i < h_; i++) {
    for (int j = 0; j < w_; j++) {
      this[i][j] = data[i * w_ + j];
    }
  }
}

template<class T>
CImage<T>::CImage(int w, int h, int max_val, FileType type, double gamma)
    : w_(w), h_(h), max_val_(max_val), type_(type), gamma_(gamma) {
  try {
    data_ = new T[w * h];
    for (int i = 0; i < w * h; i++) {
      data_[i] = {0};
    }
  } catch (std::bad_alloc &e) {
    throw CImageMemAllocException();
  }
}

template<typename T>
T CImage<T>::GetPixel(int x, int y) const {
  if (x >= 0 && y >= 0 && x < w_ && y < h_) {
    return data_[y * w_ + x];
  }
  return {0};
}

template<>
CMonoPixel CImage<CMonoPixel>::GetLinearPixel(int x, int y) const {
  return {uchar(round(pow(double(GetPixel(x, y).val) / double(max_val_), gamma_) * max_val_))};
}

template<>
CColorPixel CImage<CColorPixel>::GetLinearPixel(int x, int y) const {
  return {uchar(round(pow(double(GetPixel(x, y).r) / double(max_val_), gamma_) * max_val_)),
          uchar(round(pow(double(GetPixel(x, y).g) / double(max_val_), gamma_) * max_val_)),
          uchar(round(pow(double(GetPixel(x, y).b) / double(max_val_), gamma_) * max_val_))};
}

template<class T>
void CImage<T>::PutPixel(int x, int y, T pixel) {
  if (x >= 0 && y >= 0 && x < w_ && y < h_) {
    data_[y * w_ + x] = pixel;
  }
}

template<>
void CImage<CMonoPixel>::PutPixelWithGamma(int x, int y, CMonoPixel pixel) {
  if (x >= 0 && y >= 0 && x < w_ && y < h_) {
    data_[y * w_ + x] = {uchar(round(pow(double(pixel.val) / max_val_, 1.0/gamma_) * double(max_val_)))};
  }
}

template<>
void CImage<CColorPixel>::PutPixelWithGamma(int x, int y, CColorPixel pixel) {
  if (x >= 0 && y >= 0 && x < w_ && y < h_) {
    data_[y * w_ + x] = {uchar(round(pow(double(pixel.r) / double(max_val_), 1.0/gamma_) * double(max_val_))),
                         uchar(round(pow(double(pixel.g) / double(max_val_), 1.0/gamma_) * double(max_val_))),
                         uchar(round(pow(double(pixel.b) / double(max_val_), 1.0/gamma_) * double(max_val_)))};
  }
}

template<>
void CImage<CMonoPixel>::CorrectImageWithGamma() {
  for (int y = 0; y < GetHeight(); y++) {
    for (int x = 0; x < GetWidth(); x++) {
      PutPixel(x, y, {uchar(std::pow((double) GetPixel(x, y).val / (double) max_val_, 1.0 / gamma_) * max_val_)});
    }
  }
}

template<>
void CImage<CColorPixel>::CorrectImageWithGamma() {
  for (int y = 0; y < GetHeight(); y++) {
    for (int x = 0; x < GetWidth(); x++) {
      PutPixel(x, y, {uchar(std::pow((double) GetPixel(x, y).r / (double) max_val_, 1.0 / gamma_) * max_val_),
                      uchar(std::pow((double) GetPixel(x, y).g / (double) max_val_, 1.0 / gamma_) * max_val_),
                      uchar(std::pow((double) GetPixel(x, y).b / (double) max_val_, 1.0 / gamma_) * max_val_)});
    }
  }
}

template<typename T>
T *CImage<T>::operator[](int i) {
  return data_ + i * w_;
}

template<typename T>
int CImage<T>::GetWidth() const {
  return w_;
}

template<typename T>
int CImage<T>::GetHeight() const {
  return h_;
}

template<typename T>
int CImage<T>::GetMaxVal() const {
  return max_val_;
}

template<class T>
FileType CImage<T>::GetFileType() const {
  return type_;
}

template<typename T>
void CImage<T>::WriteImg(const std::string &fname) {
  FILE *f = fopen(fname.c_str(), "wb");
  if (!f) {
    int result = remove(fname.c_str());
    if (result != 0) {
      throw CImageFileDeleteException();
    }
    throw CImageFileOpenException();
  }
  char *head = new char[MAX_HEADER_SIZE];
  int len = snprintf(head, MAX_HEADER_SIZE, "P%i\n%i %i\n%i\n", type_, w_, h_,
                     max_val_);
  fwrite(head, 1, len, f);
  auto *buf = (uchar *) data_;
  fwrite(buf, sizeof(T), w_ * h_, f);
  delete[](head);
  fclose(f);
}

template<typename T>
bool CImage<T>::FileExists(const char *s) {
  FILE *file;
  file = fopen((const char *) s, "r");
  if (file) {
    fclose(file);
    return true;
  }
  return false;
}

template<class T>
void CImage<T>::WriteImg() {
  FILE *f = fopen(fname_.c_str(), "wb");
  char head[MAX_HEADER_SIZE];
  int len = snprintf(head, MAX_HEADER_SIZE, "P%i\n%i %i\n%i\n", type_, w_, h_,
                     max_val_);
  fwrite(head, 1, len, f);
  auto *buf = (uchar *) data_;
  fwrite(buf, sizeof(T), w_ * h_, f);
}

template<class T>
T apply_alpha(T bright, T under_color, int a, double gamma) {
  double b = under_color.val / 255.0;
  double f = bright.val / 255.0;
  double alpha = (double) a / 255.0;
  double out = pow(pow(f, gamma) * alpha + pow(b, gamma) * (1.0 - alpha), 1.0 / gamma);
  return {(uchar) (255.0 * out)};
}

template<class T>
CImage<T>::CImage(const CImage<T> &img)
    : w_(img.GetWidth()), h_(img.GetHeight()), max_val_(img.GetMaxVal()) {
  data_ = new T[w_ * h_];
  for (int i = 0; i < w_ * h_; i++) {
    data_[i] = img.data_[i];
  }
}


template<class T>
double CImage<T>::IntPart(double x) {
  return floor(x);
}

template<class T>
double CImage<T>::FloatPart(double x) {
  return x - floor(x);
}

template<>
void CImage<CMonoPixel>::FillWithGradient() {
  for (int y = 0; y < h_; y++) {
    for (int x = 0; x < w_; x++) {
      PutPixelWithGamma(x, y, {uchar((double) x / w_ * max_val_)});
    }
  }
}


template<>
void CImage<CColorPixel>::FillWithGradient() {
  for (int y = 0; y < h_; y++) {
    for (int x = 0; x < w_; x++) {
      PutPixelWithGamma(x, y, {uchar((double) x / w_ * max_val_),
                      uchar((double) x / w_ * max_val_),
                      uchar((double) x / w_ * max_val_)});
    }
  }
}



#endif //COMPUTERGEOMETRY_GRAPHICS_CIMAGE_H