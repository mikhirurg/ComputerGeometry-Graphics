//
// Created by mikha on 16.02.2020.
//
typedef unsigned char uchar;
#include <iostream>
#include <string>

using namespace std;

bool is_number(string s) {
    for (auto c : s) {
        if (!isdigit(c)) {
            return false;
        }
    }
    return true;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        cout << "Wrong number of arguments!";
        return 0;
    }
    string output(argv[2]);

    FILE* fin = fopen(argv[1], "rb");
    if (!fin) {
        cout << "Incorrect name of an input file!";
        return 0;
    }

    FILE* fout = fopen(argv[2], "wb");
    if (!fout) {
        cout << "Incorrect name of an output file!";
        return 0;
    }

    if (is_number(argv[3])) {
        int param = atoi(argv[3]);
        if (param < 0 || param > 5) {
            cout << "Incorrect transformation type";
            return 0;
        }
    } else {
        cout << "Incorrect transformation type";
        return 0;
    }

    int type, w, h, max_val;
    int i = fscanf(fin, "P%i%i%i%i\n", &type, &w, &h, &max_val);
    if (i != 4) {
        cout << "File format is not supporting";
        return 0;
    }

    uchar* data;
    if (type == 6) {
        data = new uchar[w * h * 3];
    } else {
        data = new uchar[w * h];
    }

    


}