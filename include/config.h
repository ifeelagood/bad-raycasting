#pragma once

#include "ini.h"

#include <string>
#include <vector>

class Config
{
private:
    mINI::INIFile file;
    mINI::INIStructure ini;
public:
    Config(std::string filepath) : file(filepath) { init(); };
    void init();

    unsigned int ScreenWidth, ScreenHeight;

    double MoveSpeed, RotationSpeed, SprintMult;

    unsigned short ThreadNum;

    unsigned int TextureCount;

    std::vector<std::string> TexturePaths;
};