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

    int ScreenWidth, ScreenHeight;

    int VerticalResolution;

    double MoveSpeed, RotationSpeed, SprintMultSpeed, SprintMultRotation;

    int ThreadNum;

    int TextureCount;

    uint32_t FloorColor, RoofColor, CrosshairColor;


    std::vector<std::string> TexturePaths;
};