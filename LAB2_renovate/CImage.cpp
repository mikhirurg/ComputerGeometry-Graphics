//
// Created by @mikhirurg on 11.04.2020.
//

#include <map>
#include <set>

#include "CImage.h"
#include "CImageFileOpenException.h"
#include "CImageFileDeleteException.h"
#include "CImageParamsException.h"
#include "CImageMemAllocException.h"
#include "CImageFileFormatException.h"
#include "CImageFileReadException.h"

//#define DUMP_TMP

template<typename T>
CImage<T>::CImage(const std::string &fname)
    : fname_(fname) {
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
                  int max_val)
    : fname_(fname), type_(type), w_(w), h_(h), max_val_(max_val) {
  try {
    data_ = new T[w * h];
  } catch (std::bad_alloc &e) {
    throw CImageMemAllocException();
  }
}

template<typename T>
CImage<T>::CImage(const std::string &fname, FileType type, int w, int h,
                  int max_val, const T *&data)
    : fname_(fname), type_(type), w_(w), h_(h), max_val_(max_val) {
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
CImage<T>::CImage(int w, int h, int max_val, FileType type)
    : w_(w), h_(h), max_val_(max_val), type_(type) {
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

template<typename T>
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

template<typename T>
void CImage<T>::writeImg(const std::string &fname) {
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
void CImage<T>::writeImg() {
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
void
CImage<T>::drawLine(uchar bright, double thickness, double x1, double y1,
                    double x2, double y2, double gamma) {
  if (thickness > 1) {
    int scale_x = 4;
    int scale_y = 4;

    std::vector<std::pair<double, double>>
        points = CalculateLineBorderPoints(
        thickness, x1, y1, x2, y2);
    std::pair<double, double> upper_corner = GetUpperCorner(points);

    ScaleBorderPoints(points, scale_x, scale_y);

    CImage<T>::Polygon polygon;
    polygon.GroupAdd(points);
    polygon.close();

    std::pair<double, double>
        bounds = GetScaledBounds(points, scale_x, scale_y);

    CImage<CMonoPixel>
        tmp = CImage(bounds.first, bounds.second, GetMaxVal(), P5);

    FillPolygon(polygon, tmp, {255});

#ifdef DUMP_TMP
    tmp.writeImg("tmp.pgm");
#endif
    DrawDownscaled(tmp, scale_x, scale_y, upper_corner, {bright}, gamma);
  } else {
    DrawWuLine({bright}, thickness, x1, y1, x2, y2, gamma);
  }
}

template<class T>
std::vector<std::pair<double, double>>
CImage<T>::CalculateLineBorderPoints(double thickness, double x1, double y1,
                                     double x2,
                                     double y2) {
  std::vector<std::pair<double, double>> border_points;
  x1 += 0.5;
  y1 += 0.5;
  x2 += 0.5;
  y2 += 0.5;

  double l = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
  double th2 = thickness / 2;
  double ex = (x2 - x1) / l;
  double ey = (y2 - y1) / l;
  x1 -= ex * th2;
  y1 -= ey * th2;

  x2 += ex * th2;
  y2 += ey * th2;

  if (std::abs(y2 - y1) > eps) {
    double d = (x2 - x1) / (y2 - y1);
    double a = 1 + d * d;
    double b = -(2 * d * d * x1 + 2 * x1);
    double c = x1 * x1 + d * d * x1 * x1 - thickness * thickness / 4;

    double xa_1 = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
    double xa_2 = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);

    double ya_1 = -d * xa_1 + d * x1 + y1;
    double ya_2 = -d * xa_2 + d * x1 + y1;

    b = -(2 * d * d * x2 + 2 * x2);
    c = x2 * x2 + d * d * x2 * x2 - thickness * thickness / 4;

    double xb_1 = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
    double xb_2 = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);

    double yb_1 = -d * xb_1 + d * x2 + y2;
    double yb_2 = -d * xb_2 + d * x2 + y2;

    border_points.emplace_back(xa_1, ya_1);
    border_points.emplace_back(xa_2, ya_2);
    border_points.emplace_back(xb_2, yb_2);
    border_points.emplace_back(xb_1, yb_1);
    border_points.emplace_back(xa_1, ya_1);
  } else {
    border_points.emplace_back(x1, y1 - thickness / 2);
    border_points.emplace_back(x2, y2 - thickness / 2);
    border_points.emplace_back(x2, y2 + thickness / 2);
    border_points.emplace_back(x1, y1 + thickness / 2);
    border_points.emplace_back(x1, y1 - thickness / 2);
  }
  return border_points;
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
void
CImage<T>::ScaleBorderPoints(std::vector<std::pair<double, double>> &points,
                             int scale_x, int scale_y) {
  double x_min = INT32_MAX;
  double y_min = INT32_MAX;
  double x_max = INT32_MIN;
  double y_max = INT32_MIN;
  for (std::pair<double, double> p : points) {
    x_min = std::min(x_min, p.first);
    x_max = std::max(x_max, p.first);
    y_min = std::min(y_min, p.second);
    y_max = std::max(y_max, p.second);
  }

  for (std::pair<double, double> &p : points) {
    p.first -= floor(x_min);
    p.second -= floor(y_min);
    p.first = p.first * scale_x;
    p.second = (p.second * scale_y);
  }
}

template<class T>
std::pair<double, double> CImage<T>::GetScaledBounds(
    const std::vector<std::pair<double, double>> &points,
    int scale_x,
    int scale_y) {
  double x_min = INT32_MAX;
  double y_min = INT32_MAX;
  double x_max = INT32_MIN;
  double y_max = INT32_MIN;
  for (std::pair<double, double> p : points) {
    x_min = floor(std::min(x_min, p.first));
    x_max = ceil(std::max(x_max, p.first));
    y_min = floor(std::min(y_min, p.second));
    y_max = ceil(std::max(y_max, p.second));
  }
  return std::make_pair(x_max - x_min, y_max - y_min);
}

template<class T>
std::pair<double, double> CImage<T>::GetUpperCorner(
    const std::vector<std::pair<double, double>> &points) {
  double x_min = INT32_MAX;
  double y_min = INT32_MAX;
  for (std::pair<double, double> p : points) {
    x_min = std::min(x_min, p.first);
    y_min = std::min(y_min, p.second);
  }
  return std::make_pair(floor(x_min), floor(y_min));
}

template<class T>
void
CImage<T>::DrawDownscaled(const CImage<CMonoPixel> &img, int scale_x,
                          int scale_y,
                          const std::pair<double, double> &start_coord,
                          T bright,
                          double gamma) {
  int alpha_val = 0;
  int y;
  int x;
  for (y = 0; y < img.GetHeight(); y += scale_y) {
    for (x = 0; x < img.GetWidth(); x += scale_x) {
      for (int j = 0; j < scale_y; j++) {
        for (int i = 0; i < scale_x; i++) {
          alpha_val += img.GetPixel(x + i, y + j).val;
        }
      }
      alpha_val /= (scale_x * scale_y);
      T pix = this->GetPixel(x / scale_x + (int) start_coord.first,
                             y / scale_y + (int) start_coord.second);
      this->PutPixel(x / scale_x + (int) start_coord.first,
                     y / scale_y + (int) start_coord.second,
                     apply_alpha(bright, pix, alpha_val, gamma));
      alpha_val = 0;
    }
  }
}

template<class T>
void
CImage<T>::plot(CImage &img, double x, double y, double c, CMonoPixel bright,
                double gamma) {
  if (x >= 0 && y >= 0 && x < img.GetWidth() && y < img.GetHeight()) {
    img.PutPixel(x, y, apply_alpha(bright, img.GetPixel(x, y),
                                   pow(c / img.GetMaxVal(), 1 / gamma) *
                                       img.GetMaxVal(), gamma));
  }
}

template<class T>
double CImage<T>::intPart(double x) {
  return floor(x);
}

template<class T>
double CImage<T>::FloatPart(double x) {
  return x - floor(x);
}

template<class T>
void CImage<T>::DrawWuLine(CMonoPixel brightness, double thickness, double x1,
                           double y1,
                           double x2, double y2, double gamma) {
  bool check = abs(y2 - y1) > abs(x2 - x1);
  brightness.val *= thickness;
  if (check) {
    std::swap(x1, y1);
    std::swap(x2, y2);
  }
  if (x1 > x2) {
    std::swap(x1, x2);
    std::swap(y1, y2);
  }
  double dx = x2 - x1;
  double dy = y2 - y1;
  double delta = dy / dx;

  if (dx == 0.0) {
    delta = 1.0;
  }

  if (check) {
    plot(*this, y1, x1, brightness.val, brightness, gamma);
    plot(*this, y2, x2, brightness.val, brightness, gamma);
    double y = y1 + delta;
    for (double x = x1 + 1.0; x < x2; x++) {
      plot(*this, intPart(y), x,
           (double) brightness.val * (1.0 - FloatPart(y)), brightness,
           gamma);
      plot(*this, intPart(y) + 1.0, x,
           (double) brightness.val * FloatPart(y), brightness, gamma);
      y += delta;
    }
  } else {
    plot(*this, x1, y1, brightness.val, brightness, gamma);
    plot(*this, x2, y2, brightness.val, brightness, gamma);
    double y = y1 + delta;
    for (double x = x1 + 1.0; x < x2; x++) {
      plot(*this, x, intPart(y),
           (double) brightness.val * (1.0 - FloatPart(y)), brightness,
           gamma);
      plot(*this, x, intPart(y) + 1.0,
           (double) brightness.val * FloatPart(y), brightness, gamma);
      y += delta;
    }
  }
}

template<class T>
void CImage<T>::ShiftPoints(std::vector<std::pair<double, double>> &points,
                            double shift_x, double shift_y) {
  for (std::pair<double, double> &p : points) {
    p.first += shift_x;
    p.second += shift_y;
  }
}

template<class T>
void CImage<T>::Polygon::addActive(int p, double cy) {
  Point &np = (*this)[p + 1];
  Point &pnt = (*this)[p];
  Edge &ne = active[nact];

  if (pnt.y == np.y) {
    return;
  } else {
    double dX = np.x - pnt.x;
    double dY = np.y - pnt.y;
    double dy;
    if (pnt.y < np.y) {
      ne.dir = -1;
      ne.x = pnt.x;
      dy = cy - pnt.y;
    } else {
      ne.dir = 1;
      ne.x = np.x;
      dy = cy - np.y;
    }
    ne.dx = dX / dY;
    ne.x += (dX * dy) / dY;
  }
  pnt.edge = &ne;
  active_list.push_back(&ne);
  nact++;
}

template<class T>
void CImage<T>::Polygon::sortActive() {
  struct {
    bool operator()(Edge *a, Edge *b) const {
      return a->x <= b->x;
    }
  } le;
  std::sort(active_list.begin(), active_list.end(), le);
}

template<class T>
void CImage<T>::Polygon::delActive(Edge *e) {
  active_list.erase(std::remove(active_list.begin(), active_list.end(), e),
                    active_list.end());
}

template<class T>
void CImage<T>::FillPolygon(Polygon &polygon, CImage<T> &img, T color) {
  double k, y, xl, xr;
  int drawing;
  int right_bound = img.GetWidth() - 1;
  double y_min = polygon.getYMin();
  double y_max = polygon.getYMax();
  int hash_size = (int(y_max - y_min)) + 4;

  int hash_offset = (int) ceil(y_min - 0.5 - eps);

  int counter;

  int counter_mask = -1;

  if (polygon.size() <= 1) return;
  std::map<int, std::set<int>> y_hash;

  for (int i = 0; i < polygon.size(); i++) {
    Point &next_point = polygon[i + 1];
    int key = (int) ceil(polygon[i].y - hash_offset - 0.5);
    y_hash[key].insert(i);
    polygon[i].edge = 0;
  }

  for (y = hash_offset + 0.5, k = 0;
       y <= y_max && k <= hash_size; y += 1.0, k++) {
    for (auto it = y_hash[k].begin(); it != y_hash[k].end(); ++it) {
      Point &prev = polygon[*it - 1];
      Point &next = polygon[*it + 1];
      Point &pt = polygon[*it];
      if (!prev.last_point) {
        if (prev.edge && prev.y <= y) {
          polygon.delActive(prev.edge);
          prev.edge = 0;
        } else if (prev.y > y) {
          polygon.addActive(*it - 1, y);
        }
      }

      if (!pt.last_point) {
        if (pt.edge && next.y <= y) {
          polygon.delActive(pt.edge);
          pt.edge = 0;
        } else if (next.y > y) {
          polygon.addActive(*it, y);
        }
      }
    }

    if (polygon.active_list.empty()) continue;

    polygon.sortActive();

    xl = xr = 0;
    counter = 0;
    drawing = 0;
    for (auto curEdge : polygon.active_list) {
      counter += curEdge->dir;
      if ((counter & counter_mask) && !drawing) {
        xl = floor(curEdge->x);
        drawing = 1;
      }

      if (!(counter & counter_mask) && drawing) {
        xr = floor(curEdge->x);

        if (xl <= xr) {
          for (int i = xl; i <= xr; i++) {
            img.PutPixel(i, y, color);
          }
        }
        drawing = 0;
      }

      curEdge->x += curEdge->dx;
    }

    if (drawing && xl <= right_bound) {
      for (int i = xl; i <= right_bound; i++) {
        img.PutPixel(i, y, color);
      }
    }
  }
}