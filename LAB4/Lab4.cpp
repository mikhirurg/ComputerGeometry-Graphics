//
// Created by @mikhirurg on 06.06.2020.
//

#include <iostream>
#include "CImage.h"
#include "CSpace.h"

struct globArgs {
  std::string from;
  std::string to;
  int in_count;
  std::string in_name;
  int out_count;
  std::string out_name;
} glob_args;

void log() {
  std::cout << "from_color_space: " << glob_args.from << std::endl;
  std::cout << "to_color_space: " << glob_args.to << std::endl;
  std::cout << "input_count " << glob_args.in_count << std::endl;
  std::cout << "input_name " << glob_args.in_name << std::endl;
  std::cout << "output_count " << glob_args.out_count << std::endl;
  std::cout << "output_name " << glob_args.out_name << std::endl;
}

int main(int argc, char *argv[]) {
  std::set<std::string> valid_spaces = {
      "RGB", "HSL", "HSV", "YCbCr.601", "YCbCr.709", "YCoCg", "CMY"
  };
  try {
    if (argc != 11) {
      throw CImageParamsException();
    }
    std::vector<std::string> opts;
    for (int i = 1; i < argc; i++) {
      opts.emplace_back(argv[i]);
    }
    while (!opts.empty()) {
      if (opts[0] == "-f") {
        if (opts.size() > 1 && valid_spaces.find(opts[1]) != valid_spaces.end()) {
          glob_args.from = opts[1];
        } else {
          throw CImageParamsException();
        }
        opts.erase(opts.begin());
        opts.erase(opts.begin());
        continue;
      }
      if (opts[0] == "-t") {
        if (opts.size() > 1 && valid_spaces.find(opts[1]) != valid_spaces.end()) {
          glob_args.to = opts[1];
        } else {
          throw CImageParamsException();
        }
        opts.erase(opts.begin());
        opts.erase(opts.begin());
        continue;
      }
      if (opts[0] == "-i") {
        if (opts.size() <= 2) {
          throw CImageParamsException();
        }
        try {
          glob_args.in_count = std::stoi(opts[1]);
          glob_args.in_name = opts[2];
        } catch (std::bad_cast &) {
          throw CImageParamsException();
        }
        if (!(glob_args.in_count == 1 || glob_args.in_count == 3)) {
          throw CImageParamsException();
        }
        opts.erase(opts.begin());
        opts.erase(opts.begin());
        opts.erase(opts.begin());
        continue;
      }
      if (opts[0] == "-o") {
        if (opts.size() <= 2) {
          throw CImageParamsException();
        }
        try {
          glob_args.out_count = std::stoi(opts[1]);
          glob_args.out_name = opts[2];
        } catch (std::bad_cast &) {
          throw CImageParamsException();
        }
        if (!(glob_args.out_count == 1 || glob_args.out_count == 3)) {
          throw CImageParamsException();
        }
        opts.erase(opts.begin());
        opts.erase(opts.begin());
        opts.erase(opts.begin());
      }
    }

    // log();

    std::string input_name = glob_args.in_name.substr(0, glob_args.in_name.find('.'));
    std::string output_name = glob_args.out_name.substr(0, glob_args.out_name.find('.'));

    CImage<CColorPixel> *p_img;

    if (glob_args.in_count == 3) {
      CImage<CMonoPixel> img1 = CImage<CMonoPixel>(input_name + "_1.ext", 1);
      CImage<CMonoPixel> img2 = CImage<CMonoPixel>(input_name + "_2.ext", 1);
      CImage<CMonoPixel> img3 = CImage<CMonoPixel>(input_name + "_3.ext", 1);
      p_img = new CImage<CColorPixel>(img1, img2, img3);
    } else {
      p_img = new CImage<CColorPixel>(glob_args.in_name, 1);
    }

    CImage<CColorPixel> tmp = CImage<CColorPixel>(p_img->GetWidth(),
                                                  p_img->GetHeight(),
                                                  p_img->GetMaxVal(),
                                                  p_img->GetFileType(),
                                                  p_img->GetGamma());
    if (glob_args.from == std::string("RGB") && glob_args.to == std::string("RGB")) {
      for (int y = 0; y < p_img->GetHeight(); y++) {
        for (int x = 0; x < p_img->GetWidth(); x++) {
          tmp.PutPixel(x, y, p_img->GetPixel(x, y));
        }
      }
    } else {
      if (glob_args.from == std::string("RGB")) {
        CSpace &color_space = CSpace::CSpaceByName(glob_args.to);
        for (int y = 0; y < p_img->GetHeight(); y++) {
          for (int x = 0; x < p_img->GetWidth(); x++) {
            tmp.PutPixel(x, y, color_space.FromRGB(p_img->GetPixel(x, y)));
          }
        }
      } else if (glob_args.to == std::string("RGB")) {
        CSpace &color_space = CSpace::CSpaceByName(glob_args.from);
        for (int y = 0; y < p_img->GetHeight(); y++) {
          for (int x = 0; x < p_img->GetWidth(); x++) {
            tmp.PutPixel(x, y, color_space.ToRGB(p_img->GetPixel(x, y)));
          }
        }
      } else {
        CSpace &color_space_from = CSpace::CSpaceByName(glob_args.from);
        CSpace &color_space_to = CSpace::CSpaceByName(glob_args.to);
        for (int y = 0; y < p_img->GetHeight(); y++) {
          for (int x = 0; x < p_img->GetWidth(); x++) {
            tmp.PutPixel(x, y, color_space_to.FromRGB(color_space_from.ToRGB(p_img->GetPixel(x, y))));
          }
        }
      }
    }

    if (glob_args.out_count == 3) {
      auto R = tmp.GetImageByChannel('R');
      auto G = tmp.GetImageByChannel('G');
      auto B = tmp.GetImageByChannel('B');
      R->WriteImg(output_name + "_1.ext");
      G->WriteImg(output_name + "_2.ext");
      B->WriteImg(output_name + "_3.ext");
      delete R;
      delete G;
      delete B;
    } else {
      tmp.WriteImg(glob_args.out_name);
    }

    delete (p_img);
  } catch (CImageException e) {
    std::cerr << e.getErr();
    return 1;
  }
  return 0;
}