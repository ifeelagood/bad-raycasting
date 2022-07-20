#include "map.h"

#include <iostream>
#include <fstream>

int** loadMap(std::string path, unsigned int &mapWidth, unsigned int &mapHeight)
{
    std::ifstream mapFile;
    mapFile.open(path);

    std::string line;

    // determine dimesions

    mapHeight = 0;
    if ( mapFile.is_open() )
    {
        char c;
        while (std::getline(mapFile, line))
        {
            mapHeight++;
            mapWidth = line.length();
        }
    }

    mapFile.close();

    // create map array
    int** map = new int*[mapHeight];
    for (int i = 0; i < mapHeight; i++)
    {
        map[i] = new int[mapWidth];
    }

    // populate map

    mapFile.open(path);


    int x,y;
    y = 0;
    if ( mapFile.is_open() )
    {
        while (std::getline(mapFile, line))
        {

            for (x = 0; x < mapWidth; x++)
            {
                map[y][x] = (int) line[x] - 48;
            }
            y++;
        }

    }

    return map;
}

void unloadMap(int** map, unsigned long mapWidth, unsigned long mapHeight)
{
    for (int i = 0; i < mapHeight; i++)
    {
        delete[] map[i];
    }

    delete[] map;
}
