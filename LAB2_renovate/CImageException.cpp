//
// Created by @mikhirurg on 11.04.2020.
//

#include "CImageException.h"

CImageException::CImageException(const std::string &err_m)
: err_m_(err_m)
{

}

const char *CImageException::getErr() {
    return err_m_.c_str();
}
