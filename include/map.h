#include <string>

int** loadMap(std::string path, unsigned long &mapWidth, unsigned long &mapHeight);

void unloadMap(int** map, unsigned long &mapWidth, unsigned long &mapHeight);