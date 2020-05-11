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
  explicit CImage(const std::string &fname);

  CImage(const std::string &fname, FileType type, int w, int h, int max_val);

  CImage(const std::string &fname, FileType type, int w, int h, int max_val, const T *&data);

  CImage(int w, int h, int max_val, FileType type);

  CImage(const CImage &img);

  ~CImage();

  void writeImg(const std::string &fname);

  void writeImg();

  T GetPixel(int x, int y) const;

  void PutPixel(int x, int y, T pixel);

  T *operator[](int i);

  int GetWidth() const;

  int GetHeight() const;

  int GetMaxVal() const;

  void drawLine(uchar bright, double thickness, double x1, double y1, double x2,
                double y2, double gamma);

 private:
  const double eps = 1e-10;
  const int MAX_HEADER_SIZE = 50;
  std::string fname_;
  FileType type_;
  int w_, h_;
  int max_val_;
  T *data_;

  bool FileExists(const char *s);

  struct Edge {
    double x;
    double dx;
    int dir;
  };

  struct Point {
    double x;
    double y;
    int last_point;
    Edge *edge;
  };

  struct Polygon {
    double y_min;
    double y_max;

    double getYMin() const {
      return y_min;
    }

    double getYMax() const {
      return y_max;
    }

    std::vector<Point> points;
    std::vector<Edge> active;
    std::vector<Edge *> active_list;
    int nact;

    Polygon() : points(), active(), active_list(), nact(0) {
      y_min = DBL_MAX;
      y_max = DBL_MIN;
    }

    void add(double x, double y) {
      Point p = {x, y};
      p.last_point = 0;
      if (y > y_max) y_max = y;
      if (y < y_min) y_min = y;
      points.push_back(p);
      active.push_back(Edge());
    }

    void GroupAdd(const std::vector<std::pair<double, double>> &points_group) {
      for (std::pair<double, double> p : points_group) {
        Point point = {p.first, p.second};
        point.last_point = 0;
        if (p.second > y_max) y_max = p.second;
        if (p.second < y_min) y_min = p.second;
        points.push_back(point);
        active.push_back(Edge());
      }
    }

    void close() {
      if (points.empty()) return;
      points.end()->last_point = true;
    }

    int size() {
      return points.size();
    }

    Point &operator[](int i) {
      return points[(i + points.size()) % points.size()];
    }

    void delActive(Edge *e);

    void addActive(int pnt, double cy);

    void sortActive();

  };

  void FillPolygon(Polygon &polygon, CImage<T> &img, T color);

  void plot(CImage &img, double x, double y, double c, CMonoPixel bright, double gamma);

  double intPart(double x);

  double FloatPart(double x);

  std::vector<std::pair<double, double>> CalculateLineBorderPoints(double thickness, double x1, double y1,
                                                                   double x2, double y2);
  void ScaleBorderPoints(std::vector<std::pair<double, double>> &points, int scale_x, int scale_y);

  std::pair<double, double> GetScaledBounds(const std::vector<std::pair<double, double>> &points,
                                            int scale_x,
                                            int scale_y);

  std::pair<double, double> GetUpperCorner(const std::vector<std::pair<double, double>> &points);

  void DrawDownscaled(const CImage<CMonoPixel> &img,
                      int scale_x,
                      int scale_y,
                      const std::pair<double, double> &start_coord,
                      T bright,
                      double gamma);

  void DrawWuLine(CMonoPixel bright, double thickness, double x1, double y1, double x2,
                  double y2, double gamma);

  void ShiftPoints(std::vector<std::pair<double, double>> &points, double shift_x, double shift_y);
};

#endif //COMPUTERGEOMETRY_GRAPHICS_CIMAGE_H