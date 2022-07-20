#pragma once

#include <string>

int** loadMap(std::string path, int &mapWidth, int &mapHeight);

void unloadMap(int** map, int mapWidth, int mapHeight);