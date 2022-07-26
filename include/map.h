#pragma once

#include <string>
#include <stdint.h>

int** loadMap(std::string path, int &mapWidth, int &mapHeight);

void unloadMap(int** map, int mapWidth, int mapHeight);

struct MapFile
{
    char header[4] = {'I', 'F', 'A', 'M'}; // i feel a map
    uint8_t width;
    uint8_t height;
    uint8_t **data;
};


MapFile readMapFile(std::string& filepath);
void dumpMapFile(MapFile m, std::string& filepath);