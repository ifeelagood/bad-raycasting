#include <string>
#include <iostream>
#include <fstream>

int** loadMap(std::string path, int &MAP_WIDTH, int &MAP_HEIGHT);

void deleteMap(int** map, int MAP_WIDTH, int MAP_HEIGHT);