//
// Created by mikha on 16.02.2020.
//
typedef unsigned char uchar;

#include <iostream>
#include <string>

using namespace std;

struct mono_pixel {
    uchar val;
};

struct color_pixel {
    uchar r, g, b;
};

bool is_number(const string& s) {
    for (auto c : s) {
        if (!isdigit(c)) {
            return false;
        }
    }
    return true;
}

void invert(color_pixel *data, int w, int h, uchar max_val) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; ++j) {
            data[i * w + j].r = max_val - data[i * w + j].r;
            data[i * w + j].g = max_val - data[i * w + j].g;
            data[i * w + j].b = max_val - data[i * w + j].b;
        }
    }
}


void invert(mono_pixel *data, int w, int h, uchar max_val) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; ++j) {
            data[i * w + j].val = max_val - data[i * w + j].val;
        }
    }
}

template <typename T>
void swap_pixels(T *data, int w, int h, int x1, int y1, int x2, int y2) {
    swap(data[y1 * w + x1], data[y2 * w + x2]);
}

template <typename T>
void horizontal_flip(T *data, int w, int h) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w / 2; ++j) {
            swap_pixels(data, w, h, j, i, w - j, i);
        }
    }
}

template <typename T>
void vertical_flip(T *data, int w, int h) {
    for (int i = 0; i < h / 2; i++) {
        for (int j = 0; j < w; ++j) {
            swap_pixels(data, w, h, j, i, j, h - i);
        }
    }
}

template <typename T>
void rotate_left(T *data, int &w, int &h) {
    auto *tmp = new T[w * h];
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            tmp[i * w + j] = data[i * w + j];
        }
    }
    int k = 0;
    for (int j = 0; j < w; j++) {
        for (int i = h - 1; i >= 0; i--) {
            data[k] = tmp[i * w + j];
            k++;
        }
    }
    swap(w, h);
    delete[](tmp);
}

template <typename T>
void rotate_right(T *data, int &w, int &h) {
    auto *tmp = new T[w * h];
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            tmp[i * w + j] = data[i * w + j];
        }
    }
    int k = 0;
    for (int j = w - 1; j >= 0; j--) {
        for (int i = 0; i < h; i++) {
            data[k] = tmp[i * w + j];
            k++;
        }
    }
    swap(w, h);
    delete[](tmp);
}

void write_file(FILE *f, mono_pixel *data, int w, int h, int type, int max_val) {
    string header =
            "P" + to_string(type) + "\n" + to_string(w) + " " + to_string(h) +
            "\n" + to_string(max_val) + "\n";
    fwrite(header.c_str(), 1, header.size(), f);
    auto *buf = (uchar *) data;
    fwrite(buf, 1, w * h, f);
}

void write_file(FILE *f, color_pixel *data, int w, int h, int type, int max_val) {
    string header =
            "P" + to_string(type) + "\n" + to_string(w) + " " + to_string(h) +
            "\n" + to_string(max_val) + "\n";
    fwrite(header.c_str(), 1, header.size(), f);
    auto *buf = (uchar *) data;
    fwrite(buf, 1, w * h * 3, f);
}

template <typename T>
void do_transform(T *pixels, int &w, int &h, int max_val, int param) {
    switch (param) {
        case 0:
            invert(pixels, w, h, max_val);
            break;
        case 1:
            horizontal_flip(pixels, w, h);
            break;
        case 2:
            vertical_flip(pixels, w, h);
            break;
        case 3:
            rotate_left(pixels, w, h);
            break;
        case 4:
            rotate_right(pixels, w, h);
            break;
        default:
            break;
    }

}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        cout << "Wrong number of arguments!";
        return 0;
    }

    FILE *fin = fopen(argv[1], "rb");
    if (!fin) {
        cout << "Incorrect name of an input file!";
        fclose(fin);
        return 0;
    }

    FILE *fout = fopen(argv[2], "wb");
    if (!fout) {
        cout << "Incorrect name of an output file!";
        fclose(fin);
        fclose(fout);
        remove(argv[2]);
        return 0;
    }
    int param = 0;
    if (is_number(argv[3])) {
        param = atoi(argv[3]);
        if (param < 0 || param > 4) {
            cout << "Incorrect transformation type";
            fclose(fin);
            fclose(fout);
            remove(argv[2]);
            return 0;
        }
    } else {
        cout << "Incorrect transformation type";
        fclose(fin);
        fclose(fout);
        remove(argv[2]);
        return 0;
    }

    int type, w, h, max_val;
    int i = fscanf(fin, "P%i%i%i%i\n", &type, &w, &h, &max_val);
    if (i != 4) {
        cout << "File format is not supporting";
        fclose(fin);
        fclose(fout);
        remove(argv[2]);
        return 0;
    }
    if (type != 5 && type != 6) {
        cout << "File format is not supporting";
        fclose(fin);
        fclose(fout);
        remove(argv[2]);
        return 0;
    }

    uchar *data;

    if (type == 6) {
        data = new uchar[w * h * 3];
        fread(data, 2, w * h * 3, fin);
        auto *pixels = (color_pixel *) data;
        do_transform(pixels, w, h, max_val, param);
        write_file(fout, pixels, w, h, type, max_val);
    } else {
        data = new uchar[w * h];
        fread(data, 2, w * h, fin);
        auto *pixels = (mono_pixel *) data;
        do_transform(pixels, w, h, max_val, param);
        write_file(fout, pixels, w, h, type, max_val);
    }

    fclose(fout);
}