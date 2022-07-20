#include "config.h"

void Config::init()
{
    file.read(ini);

    //-----[display]-----
    ScreenWidth = std::stoi(ini["display"]["iWidth"]);
    ScreenHeight = std::stoi(ini["display"]["iHeight"]);


    //-----[gameplay]-----
    MoveSpeed = std::stod(ini["gameplay"]["fMoveSpeed"]);
    RotationSpeed = std::stod(ini["gameplay"]["fRotationSpeed"]);
    SprintMult = std::stod(ini["gameplay"]["fSprintMult"]);

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

}