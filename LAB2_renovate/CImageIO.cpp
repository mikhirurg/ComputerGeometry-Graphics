//
// Created by @mikhirurg on 11.04.2020.
//

#include "CImageIO.h"
#include "CImageFileOpenException.h"
#include "CImageFileDeleteException.h"
#include "CImageParamsException.h"
#include "CImageMemAllocException.h"
#include "CImageFileFormatException.h"
#include "CImageFileReadException.h"

template <typename T>
CImageIO<T>::CImageIO(const std::string& fname)
: fname_(fname)
{
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
        data_ = new T[w_*h_];
        int check = fread(data_, sizeof(T), w_ * h_, f);
        if (check != w_ * h_) {
            fclose(f);
            throw CImageFileReadException();
        }
    } catch (std::bad_alloc&) {
        throw CImageMemAllocException();
    }
    fclose(f);
}

template <typename T>
CImageIO<T>::~CImageIO() {
    delete[](data_);
}

template<typename T>
CImageIO<T>::CImageIO(const std::string &fname, file_type type, int w, int h,
                      int max_val)
: fname_(fname)
, type_(type)
, w_(w)
, h_(h)
, max_val_(max_val)
{
    try {
        data_ = new T[w * h];
    } catch (std::bad_alloc& e) {
        throw CImageMemAllocException();
    }
}

template<typename T>
CImageIO<T>::CImageIO(const std::string &fname, file_type type, int w, int h,
                      int max_val, const T *&data)
: fname_(fname)
, type_(type)
, w_(w)
, h_(h)
, max_val_(max_val)
{
    try {
        data_ = new T[w_ * h_];
    } catch (std::bad_alloc& e) {
        throw CImageMemAllocException();
    }
    for (int i = 0; i < h_; i++) {
        for (int j = 0; j < w_; j++) {
            this[i][j] = data[i * w_ + j];
        }
    }
}

template<typename T>
T CImageIO<T>::getPixel(int x, int y) {
    if (x < 0 || y < 0 || x >= w_ || y >=h_) {
        throw CImageParamsException();
    }
    return data_[y * w_ + x];
}

template<typename T>
void CImageIO<T>::putPixel(int x, int y, T pixel) {
    if (x < 0 || y < 0 || x >= w_ || y >=h_) {
        throw CImageParamsException();
    }
    data_[y*w_ + x] = pixel;
}

template<typename T>
T *CImageIO<T>::operator[](int i) {
    return data_ + i * w_;
}

template<typename T>
int CImageIO<T>::getWidth() {
    return w_;
}

template<typename T>
int CImageIO<T>::getHeight() {
    return h_;
}

template<typename T>
int CImageIO<T>::getMaxVal() {
    return max_val_;
}

template<typename T>
void CImageIO<T>::writeImg(const std::string &fname) {
    FILE *f = fopen(fname.c_str(), "wb");
    if (!f) {
        int result = remove(fname.c_str());
        if (result != 0) {
            throw CImageFileDeleteException();
        }
        throw CImageFileOpenException();
    }
    char head[MAX_HEADER_SIZE];
    int len = snprintf(head, MAX_HEADER_SIZE, "P%i\n%i %i\n%i\n", type_, w_, h_, max_val_);
    fwrite(head, 1, len, f);
    auto *buf = (uchar *) data_;
    fwrite(buf, sizeof(T), w_*h_, f);
}

template<typename T>
bool CImageIO<T>::file_exists(const char *s) {
    FILE *file;
    file = fopen((const char *) s, "r");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}

template<class T>
void CImageIO<T>::writeImg() {
    FILE *f = fopen(fname_.c_str(), "wb");
    char head[MAX_HEADER_SIZE];
    int len = snprintf(head, MAX_HEADER_SIZE, "P%i\n%i %i\n%i\n", type_, w_, h_, max_val_);
    fwrite(head, 1, len, f);
    auto *buf = (uchar *) data_;
    fwrite(buf, sizeof(T), w_*h_, f);
}
