//
// Created by mikha on 16.02.2020.
//
typedef unsigned char uchar;

#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <algorithm>

using namespace std;

enum transform_type {
    INVERSION,
    H_FLIP,
    V_FLIP,
    C_ROTATE,
    AC_ROTATE
};

enum error {
    FILE_OPEN_ERR,
    FILE_FORMAT_ERR,
    MEMORY_ALLOCATION_ERR,
    PARAMS_ERR,
    FILE_DELETE_ERR
};

const int MAX_HEADER_SIZE = 50;

enum file_type {
    P5 = 5,
    P6
};

struct mono_pixel {
    uchar val;
};

struct color_pixel {
    uchar r, g, b;
};

struct pixel {
    double x, y;
};

template<typename T>
struct image {
    file_type type;
    int w, h;
    int max_val;
    T *data;
};

bool is_number(const string &s) {
    for (auto c : s) {
        if (!isdigit(c)) {
            return false;
        }
    }
    return true;
}


bool file_exists(const char *s) {
    FILE *file;
    file = fopen((const char *) s, "r");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}

void print_err(error err) {
    switch (err) {
        case FILE_OPEN_ERR:
            cerr << "Can't open file!";
            break;
        case FILE_FORMAT_ERR:
            cerr << "Unsupported file format!";
            break;
        case MEMORY_ALLOCATION_ERR:
            cerr << "Can't allocate memory!";
            break;
        case PARAMS_ERR:
            cerr << "Wrong params!";
            break;
        case FILE_DELETE_ERR:
            cerr << "File delete error";
            break;
        default:
            cerr << "ERROR!";
            break;
    }
}

template<typename T>
void write_file(FILE *f, const image<T> &img) {
    char head[MAX_HEADER_SIZE];
    int len = snprintf(head, MAX_HEADER_SIZE, "P%i\n%i %i\n%i\n", img.type,
                       img.w, img.h, img.max_val);
    fwrite(head, 1, len, f);
    auto *buf = (uchar *) img.data;
    fwrite(buf, sizeof(T), img.w * img.h, f);
}

void plot(image<mono_pixel> &img, double x, double y, double c, double gamma) {
    if (x >= 0 && y >= 0 && x < img.w && y < img.h) {
        img.data[(int) (y * img.w + x)].val =
                pow(c / img.max_val, 1 / gamma) * img.max_val;
        cout << x << " " << y << endl;
    }
}

double intPart(double x) {
    return floor(x);
}

double floatPart(double x) {
    return x - floor(x);
}

struct comp_x{
    bool operator()(pair<double, double> o1, pair<double, double> o2){
        return o1.first > o1.first;
    }
};

struct comp_y{
    bool operator()(pair<double, double> o1, pair<double, double> o2){
        return o1.second > o1.second;
    }
};


template<typename T>
void draw_line(image<T> &img, double brightness, double line_width, double x1,
               double y1, double x2, double y2, double gamma) {
    bool check = abs(y2 - y1) > abs(x2 - x1);
    brightness = pow((brightness / 255.0), gamma) * 255.0;
    if (check) {
        swap(x1, y1);
        swap(x2, y2);
    }
    if (x1 > x2) {
        swap(x1, x2);
        swap(y1, y2);
    }
    double dx = x2 - x1;
    double dy = y2 - y1;
    double delta = dy / dx;

    if (dx == 0.0) {
        delta = 1.0;
    }
    cout << delta << endl << endl;

    double alpha = atan(delta);

    vector<pair<double, double>> dots;

    pair<double, double> dot = {x1 + (line_width / 2) * cos(alpha), y1 + (line_width / 2) * sin(alpha)};
    dots.push_back(dot);
    dot = {x1 - (line_width / 2) * cos(alpha), y1 - (line_width / 2) * sin(alpha)};
    dots.push_back(dot);
    dot = {x2 + (line_width / 2) * cos(alpha), y2 + (line_width / 2) * sin(alpha)};
    dots.push_back(dot);
    dot = {x2 - (line_width / 2) * cos(alpha), y2 - (line_width / 2) * sin(alpha)};
    dots.push_back(dot);

    double max_x = INT32_MIN;
    double max_y = INT32_MIN;
    double min_x = INT32_MAX;
    double min_y = INT32_MAX;

    for (int i = 0; i < dots.size(); i++) {
        if (dots[i].first > max_x) {
            max_x = dots[i].first;
        }
        if (dots[i].first < min_x) {
            min_x = dots[i].first;
        }
        if (dots[i].second > max_y) {
            max_y = dots[i].second;
        }
        if (dots[i].second < min_y) {
            min_y = dots[i].second;
        }
    }

    bool line = false;

    sort(dots.begin(), dots.end(), comp_y());

    for (int step_y = min_y; step_y < max_y; step_y++) {
        double x_start = max((dots[2].first - dots[0].first) * (step_y - dots[0].second) / (dots[2].second - dots[0].second) + dots[0].first,
                             (dots[2].first - dots[3].first) * (step_y - dots[2].second) / (dots[3].second - dots[2].second) + dots[2].first);
        double x_end = min((dots[0].first - dots[1].first) * (step_y - dots[0].second) / (dots[1].second - dots[1].second) + dots[0].first,
                           (dots[1].first - dots[1].first) * (step_y - dots[3].second) / (dots[3].second - dots[1].second) + dots[1].first);
        for (double step_x = x_start; step_x < x_end; step_x++) {
            plot(img, step_x, step_y, brightness, gamma);
        }
    }



    /*if (check) {
        double y = y1 + delta;
        for (int x = round(x1); x < round(x2); x++) {
            for (int y_step = intPart(y - (line_width - 1) / 2); y_step <= (line_width + intPart(y - (line_width - 1) / 2)); y_step++) {
                plot(img, y_step, x, brightness * min(((line_width + 1) / 2 - abs(y - y_step)), 1.0), gamma);
            }
            y += delta;
        }
    } else {
        double y = y1 + delta;
        for (int x = round(x1); x < round(x2); x++) {
            for (int y_step = intPart(y - (line_width - 1) / 2); y_step <= (line_width + intPart(y - (line_width - 1) / 2)); y_step++) {
                plot(img, x, y_step, brightness * min(((line_width + 1) / 2 - abs(y - y_step)), 1.0), gamma);
            }
            y += delta;
        }
    }*/
}

template<typename T>
void
process_file(image<T> &img, int brightness, double line_width, double start_x,
             double start_y, double end_x, double end_y, double gamma,
             FILE *fin, FILE *fout,
             char *out_name, bool out_exists) {
    int check = fread(img.data, sizeof(T), img.w * img.h, fin);
    if (check != img.w * img.h) {
        print_err(FILE_FORMAT_ERR);
        fclose(fin);
        fclose(fout);
        if (!out_exists) {
            int result = remove(out_name);
            if (result != 0) {
                print_err(FILE_DELETE_ERR);
            }
        }
        delete[](img.data);
        exit(1);
    }
    draw_line(img, brightness, line_width, start_x, start_y, end_x, end_y,
              gamma);
    write_file(fout, img);
    delete[](img.data);
}

int main(int argc, char *argv[]) {
    if (argc != 10) {
        print_err(PARAMS_ERR);
        return 1;
    }

    FILE *fin = fopen(argv[1], "rb");
    if (!fin) {
        print_err(FILE_OPEN_ERR);
        return 1;
    }

    bool out_exists = file_exists(argv[2]);
    FILE *fout = fopen(argv[2], "wb");
    if (!fout) {
        print_err(FILE_OPEN_ERR);
        fclose(fin);
        int result = remove(argv[2]);
        if (result != 0) {
            print_err(FILE_DELETE_ERR);
        }
        return 1;
    }

    int brightness;
    double line_width, x1, y1, x2, y2, gamma;

    try {
        brightness = stoi(argv[3]);
        line_width = stod(argv[4]);
        x1 = stod(argv[5]);
        y1 = stod(argv[6]);
        x2 = stod(argv[7]);
        y2 = stod(argv[8]);
        gamma = stod(argv[9]);
    } catch (invalid_argument &) {
        print_err(PARAMS_ERR);
        return 1;
    }

    int w, h, max_val;
    file_type type;
    int i = fscanf(fin, "P%i%i%i%i\n", &type, &w, &h, &max_val); // NOLINT(cert-err34-c)
    if (i != 4 || w <= 0 || h <= 0 || max_val <= 0) {
        print_err(FILE_FORMAT_ERR);
        fclose(fin);
        fclose(fout);
        int result = remove(argv[2]);
        if (result != 0) {
            print_err(FILE_DELETE_ERR);
        }
        return 1;
    }
    if (type != P5) {
        print_err(FILE_FORMAT_ERR);
        fclose(fin);
        fclose(fout);
        int result = remove(argv[2]);
        if (result != 0) {
            print_err(FILE_DELETE_ERR);
        }
        return 1;
    }
    try {
        auto data = new mono_pixel[w * h];
        image<mono_pixel> img = {type, w, h, max_val, data};
        process_file(img, brightness, line_width, x1, y1, x2,
                     y2, gamma, fin, fout, argv[2], out_exists);
        fclose(fout);
    } catch (bad_alloc &) {
        print_err(MEMORY_ALLOCATION_ERR);
        fclose(fin);
        fclose(fout);
        int result = remove(argv[2]);
        if (result != 0) {
            print_err(FILE_DELETE_ERR);
        }
        return 1;
    }
    return 0;
}
