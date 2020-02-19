//
// Created by mikha on 16.02.2020.
//
typedef unsigned char uchar;

#include <iostream>
#include <string>

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
    PARAMS_ERR
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
            cout << "Can't open file!";
            break;
        case FILE_FORMAT_ERR:
            cout << "Unsupported file format!";
            break;
        case MEMORY_ALLOCATION_ERR:
            cout << "Can't allocate memory!";
            break;
        case PARAMS_ERR:
            cout << "Wrong params!";
            break;
        default:
            cout << "ERROR!";
            break;
    }
}

void invert(image<color_pixel> &img) {
    for (int i = 0; i < img.h; i++) {
        for (int j = 0; j < img.w; ++j) {
            img.data[i * img.w + j].r = img.max_val - img.data[i * img.w + j].r;
            img.data[i * img.w + j].g = img.max_val - img.data[i * img.w + j].g;
            img.data[i * img.w + j].b = img.max_val - img.data[i * img.w + j].b;
        }
    }
}


void invert(image<mono_pixel> &img) {
    for (int i = 0; i < img.h; i++) {
        for (int j = 0; j < img.w; ++j) {
            img.data[i * img.w + j].val =
                    img.max_val - img.data[i * img.w + j].val;
        }
    }
}

template<typename T>
void swap_pixels(image<T> &img, int x1, int y1, int x2, int y2) {
    if (x1 < img.w && x1 >= 0 && y1 < img.h && y1 >= 0 && x2 < img.w &&
        x2 >= 0 &&
        y2 < img.h && y2 >= 0) {
        swap(img.data[y1 * img.w + x1], img.data[y2 * img.w + x2]);
    }
}

template<typename T>
void horizontal_flip(image<T> &img) {
    for (int i = 0; i < img.h; i++) {
        for (int j = 0; j < img.w / 2; j++) {
            swap_pixels(img, j, i, img.w - j - 1, i);
        }
    }
}

template<typename T>
void vertical_flip(image<T> &img) {
    for (int i = 0; i < img.h / 2; i++) {
        for (int j = 0; j < img.w; j++) {
            swap_pixels(img, j, i, j, img.h - i - 1);
        }
    }
}

template<typename T>
void rotate_left(image<T> &img) {
    auto *tmp = new T[img.w * img.h];
    int k = 0;
    for (int j = 0; j < img.w; j++) {
        for (int i = img.h - 1; i >= 0; i--) {
            tmp[k] = img.data[i * img.w + j];
            k++;
        }
    }
    delete[](img.data);
    img.data = tmp;
    swap(img.w, img.h);
}

template<typename T>
void rotate_right(image<T> &img) {
    auto *tmp = new T[img.w * img.h];
    int k = 0;
    for (int j = img.w - 1; j >= 0; j--) {
        for (int i = 0; i < img.h; i++) {
            tmp[k] = img.data[i * img.w + j];
            k++;
        }
    }
    delete[](img.data);
    img.data = tmp;
    swap(img.w, img.h);
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

template<typename T>
void do_transform(image<T> &img, transform_type param) {
    switch (param) {
        case INVERSION:
            invert(img);
            break;
        case H_FLIP:
            horizontal_flip(img);
            break;
        case V_FLIP:
            vertical_flip(img);
            break;
        case C_ROTATE:
            rotate_left(img);
            break;
        case AC_ROTATE:
            rotate_right(img);
            break;
        default:
            // Never happen
            break;
    }
}

template<typename T>
void process_file(image<T> &img, transform_type param, FILE *fin, FILE *fout,
                  char* out_name, bool out_exists)
{
    int check = fread(img.data, sizeof(T), img.w * img.h, fin);
    if (check < sizeof(T) * img.w * img.h) {
        print_err(FILE_FORMAT_ERR);
        fclose(fin);
        if (!out_exists) {
            remove(out_name);
        }
        exit(1);
    }
    do_transform(img, param);
    write_file(fout, img);
    delete[](img.data);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        print_err(PARAMS_ERR);
        return 1;
    }

    FILE *fin = fopen(argv[1], "rb");
    if (!fin) {
        print_err(FILE_OPEN_ERR);
        fclose(fin);
        return 1;
    }

    bool out_exists = file_exists(argv[2]);

    transform_type param;
    if (is_number(argv[3])) {
        char *endptr;
        int arg3 = (int) strtol(argv[3], &endptr, 10);
        if (argv[3] == endptr || arg3 < 0 || arg3 > 4) {
            print_err(PARAMS_ERR);
            fclose(fin);
            if (!out_exists) {
                remove(argv[2]);
            }
            return 1;
        }
        param = (transform_type) arg3;
    } else {
        print_err(PARAMS_ERR);
        fclose(fin);
        if (!out_exists) {
            remove(argv[2]);
        }
        return 1;
    }

    FILE *fout = fopen(argv[2], "wb");
    if (!fout) {
        print_err(FILE_OPEN_ERR);
        fclose(fin);
        fclose(fout);
        return 1;
    }

    int w, h, max_val;
    file_type type;
    int i = fscanf(fin, "P%i%i%i%i\n", &type, &w, &h, &max_val); // NOLINT(cert-err34-c)
    if (i != 4 || w <= 0 || h <= 0 || max_val <= 0) {
        print_err(FILE_FORMAT_ERR);
        fclose(fin);
        fclose(fout);
        if (!out_exists) {
            remove(argv[2]);
        }
        return 1;
    }
    if (type != P5 && type != P6) {
        print_err(FILE_FORMAT_ERR);
        fclose(fin);
        fclose(fout);
        if (!out_exists) {
            remove(argv[2]);
        }
        return 1;
    }

    switch (type) {
        case P5: {
            try {
                auto data = new mono_pixel[w * h];
                image<mono_pixel> img = {type, w, h, max_val, data};
                process_file(img, param, fin, fout, argv[2], out_exists);
                fclose(fout);
            } catch (bad_alloc &) {
                print_err(MEMORY_ALLOCATION_ERR);
                fclose(fin);
                fclose(fout);
                if (!out_exists) {
                    remove(argv[2]);
                }
                return 1;
            }
            break;
        }
        case P6: {
            try {
                auto data = new color_pixel[w * h];
                image<color_pixel> img = {type, w, h, max_val, data};
                process_file(img, param, fin, fout, argv[2], out_exists);
                fclose(fout);
            } catch (bad_alloc &) {
                print_err(MEMORY_ALLOCATION_ERR);
                fclose(fin);
                fclose(fout);
                if (!out_exists) {
                    remove(argv[2]);
                }
                return 1;
            }
            break;
        }
        default:
            // Never happen
            break;
    }
    return 0;
}