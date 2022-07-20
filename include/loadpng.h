#pragma once

#define cimg_display 0

#include <vector>
#include <string>
#include <bits/stdint-uintn.h>
#include <CImg.h>
#include <png.h>


void loadPNG(std::vector<uint32_t>& out, unsigned int& w, unsigned int& h, const std::string filename);