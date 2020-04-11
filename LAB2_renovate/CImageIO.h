//
// Created by @mikhirurg on 11.04.2020.
//

#ifndef COMPUTERGEOMETRY_GRAPHICS_CIMAGEIO_H
#define COMPUTERGEOMETRY_GRAPHICS_CIMAGEIO_H
typedef unsigned char uchar;
#include <string>

enum file_type {
    P5 = 5,
    P6
};

struct CMonoPixel {
    uchar val;
};

struct CColorPixel {
    uchar r, g, b;
};

template <class T>
class CImageIO {
public:
    CImageIO(const std::string& fname);

    CImageIO(const std::string& fname, file_type type, int w, int h, int max_val);

    CImageIO(const std::string& fname, file_type type, int w, int h, int max_val, const T* &data);

    ~CImageIO();

    void readImg();

    void writeImg(const std::string& fname);

    void writeImg();

    T getPixel(int x, int y);

    void putPixel(int x, int y, T pixel);

    T* operator[](int i);

    int getWidth();

    int getHeight();

    int getMaxVal();

private:
    const int MAX_HEADER_SIZE = 50;
    std::string fname_;
    file_type type_;
    int w_, h_;
    int max_val_;
    T *data_;
    bool file_exists(const char *s);
};


#endif //COMPUTERGEOMETRY_GRAPHICS_CIMAGEIO_H
