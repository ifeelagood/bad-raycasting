#include "config.h"


uint32_t stoui32_t(std::string str)
{
    uint32_t n = static_cast<uint32_t>(std::stoul(str));
    return n;
}

void Config::init()
{
    file.read(ini);

    //-----[display]-----
    ScreenWidth = std::stoi(ini["display"]["iWidth"]);
    ScreenHeight = std::stoi(ini["display"]["iHeight"]);
    VerticalResolution = std::stoi(ini["display"]["iVerticalResolution"]);


    //-----[gameplay]-----
    MoveSpeed = std::stod(ini["gameplay"]["fMoveSpeed"]);
    RotationSpeed = std::stod(ini["gameplay"]["fRotationSpeed"]);
    SprintMultSpeed = std::stod(ini["gameplay"]["fSprintMultSpeed"]);
    SprintMultRotation = std::stod(ini["gameplay"]["fSprintMultRotation"]);

    //-----[engine]-----
    ThreadNum = std::stoi(ini["engine"]["iThreadNum"]);

    //-----[textures]-----
    TextureCount = std::stoi(ini["textures"]["iTextureCount"]);

    auto const& textureCollection = ini["textures"];

    bool i = false;
    for (auto const& it : textureCollection)
    {
        if (!i) { i = true; continue; } // skip texture count record
        // auto const& key = it.first;
        auto const& value = it.second;
        TexturePaths.push_back(value);
    }


    //-----[color]-----
    CrosshairColor = stoui32_t(ini["color"]["uCrosshair"]);
    FloorColor = stoui32_t(ini["color"]["uFloor"]);
    RoofColor = stoui32_t(ini["color"]["uRoof"]);
}