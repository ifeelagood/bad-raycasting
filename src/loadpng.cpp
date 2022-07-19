#include "loadpng.h"

#include <iostream>
#include <bitset>
using namespace cimg_library;


void loadPNG(std::vector<uint32_t>& out, int& w, int& h, const std::string filename)
{
    CImg<uint8_t> img(filename.c_str());

    w = img.width();
    h = img.height();


    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            uint8_t r = img.atXY(x, y, 0);
            uint8_t g = img.atXY(x, y, 1);
            uint8_t b = img.atXY(x, y, 2);
            uint8_t a = img.atXY(x, y, 3);

            uint32_t pixel = (a | (b << 8) | (g << 16) | (r << 24));
            out[x+y*h] = pixel;
        }
    }

}