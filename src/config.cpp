#include "config.h"


void Config::init()
{
    file.read(ini);

    //-----[display]-----
    ScreenWidth = (unsigned int) std::stoi(ini["display"]["iWidth"]);
    ScreenHeight = (unsigned int) std::stoi(ini["display"]["iHeight"]);


    //-----[gameplay]-----
    MoveSpeed = std::stod(ini["gameplay"]["fMoveSpeed"]);
    RotationSpeed = std::stod(ini["gameplay"]["fRotationSpeed"]);
    SprintMult = std::stod(ini["gameplaye"]["fSprintMult"]);

    //-----[engine]-----
    ThreadNum = (unsigned short) std::stoi(ini["engine"]["iThreadNum"]);

    //-----[textures]-----
    TextureCount = (unsigned int) std::stoi(ini["textures"]["iTextureCount"]);

    auto const& textureCollection = ini["textures"];

    for (auto const& it : textureCollection)
    {
        // auto const& key = it.first;
        auto const& value = it.second;
        TexturePaths.push_back(value);
    }

}