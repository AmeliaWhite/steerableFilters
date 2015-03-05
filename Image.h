#ifndef IMAGE_H
#define IMAGE_H

#define cimg_display_type 0
#include <CImg.h>

typedef cimg_library::CImg<unsigned char> Image;
typedef cimg_library::CImg<int> IImage;
typedef cimg_library::CImg<double> DImage;

template<class T> struct Matrix : cimg_library::CImg<T>
{
    Matrix() : cimg_library::CImg<T>() {}
    Matrix(int dimx, int dimy, int dimz = 1, int dimv = 1) : cimg_library::CImg<T>(dimx, dimy, dimz, dimv) {}
};

#endif // IMAGE_H
