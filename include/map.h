#pragma once

#include <string>

int** loadMap(std::string path, unsigned int &mapWidth, unsigned int &mapHeight);

void unloadMap(int** map, unsigned int &mapWidth, unsigned int &mapHeight);