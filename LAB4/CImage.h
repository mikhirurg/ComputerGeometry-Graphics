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
#include "CPixel.h"
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

template<class T>
class CImage {
 public:
  explicit CImage(const std::string &fname, double gamma);

  CImage();

  CImage(const std::string &fname, FileType type, int w, int h, int max_val, double gamma);

  CImage(const std::string &fname, FileType type, int w, int h, int max_val, const T *&data, double gamma);

  CImage(int w, int h, int max_val, FileType type, double gamma);

  CImage(const CImage<CMonoPixel>& img1, const CImage<CMonoPixel>& img2, const CImage<CMonoPixel>& img3);

  CImage(const CImage &img);

  ~CImage();

  void WriteImg(const std::string &fname);

  void WriteImg();

  T GetPixel(int x, int y) const;

  void PutPixel(int x, int y, T pixel);

  T *operator[](int i);

  int GetWidth() const;

  int GetHeight() const;

  int GetMaxVal() const;

  double GetGamma() const;

  FileType GetFileType() const;

  CMonoPixel Clamp(double val);

  CColorPixel Clamp(double val_r, double val_g, double val_b);

  CImage<CMonoPixel>* GetImageByChannel(char channel_name);

 private:
  double eps = 1e-10;
  int MAX_HEADER_SIZE = 50;
  std::string fname_;
  FileType type_;
  int w_, h_;
  int max_val_;
  T *data_;
  double gamma_;

  bool FileExists(const char *s);

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

template<>
CImage<CColorPixel>::CImage(const CImage<CMonoPixel>& img1, const CImage<CMonoPixel>& img2, const CImage<CMonoPixel>& img3)
    : w_(img1.GetWidth()), h_(img2.GetHeight()), max_val_(img1.GetMaxVal()), type_(P6), gamma_(img1.GetGamma()) {
  try {
    data_ = new CColorPixel[w_ * h_];
    for (int y = 0; y < h_; y++) {
      for (int x = 0; x < w_; x++) {
        data_[y * w_ + x] = CColorPixel{img1.GetPixel(x, y).val, img2.GetPixel(x, y).val, img3.GetPixel(x, y).val};
      }
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

template<class T>
void CImage<T>::PutPixel(int x, int y, T pixel) {
  if (x >= 0 && y >= 0 && x < w_ && y < h_) {
    data_[y * w_ + x] = pixel;
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
CImage<T>::CImage(const CImage<T> &img)
    : w_(img.GetWidth()), h_(img.GetHeight()), max_val_(img.GetMaxVal()), gamma_(img.GetGamma()) {
  data_ = new T[w_ * h_];
  for (int i = 0; i < w_ * h_; i++) {
    data_[i] = img.data_[i];
  }
}
template<class T>
CMonoPixel CImage<T>::Clamp(double val) {
  return {uchar(std::min(std::max(val, 0.0), double(max_val_)))};
}
template<class T>
CColorPixel CImage<T>::Clamp(double val_r, double val_g, double val_b) {
  return {uchar(std::min(std::max(val_r, 0.0), double(max_val_))),
          uchar(std::min(std::max(val_g, 0.0), double(max_val_))),
          uchar(std::min(std::max(val_b, 0.0), double(max_val_)))};
}
template<class T>
double CImage<T>::GetGamma() const {
  return gamma_;
}

template<class T>
CImage<T>::CImage() {

}

template<>
CImage<CMonoPixel>* CImage<CColorPixel>::GetImageByChannel(char channel_name) {
  CImage<CMonoPixel>* img = new CImage<CMonoPixel>(w_, h_, max_val_, P5, gamma_);
  switch (channel_name) {
    case 'R': {
      for (int y = 0; y < h_; y++) {
        for (int x = 0; x < w_; x++) {
          img->PutPixel(x, y, {GetPixel(x, y).r});
        }
      }
    }
    break;
    case 'G': {
      for (int y = 0; y < h_; y++) {
        for (int x = 0; x < w_; x++) {
          img->PutPixel(x, y, {GetPixel(x, y).g});
        }
      }
    }
    break;
    case 'B': {
      for (int y = 0; y < h_; y++) {
        for (int x = 0; x < w_; x++) {
          img->PutPixel(x, y, {GetPixel(x, y).b});
        }
      }
    }
    break;
    default: {
      //Never happen
    }
  }
  return img;
}

#endif //COMPUTERGEOMETRY_GRAPHICS_CIMAGE_H