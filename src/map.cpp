#include "map.h"

#include <iostream>
#include <fstream>

int** loadMap(std::string path, int &mapWidth, int &mapHeight)
{
    std::ifstream mapFile;
    mapFile.open(path);

    std::string line;

    // determine dimensions by reading full file
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

    // create new P2P map array
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
                int i;
                int ascii = (int) line[x];

                if (ascii == 32) { i = 0; } // space can also be zero for easier designing
                else             { i = ascii - 48; } // 48 is first ascii digit

                map[y][x] = i;
            }
            y++;
        }
    }

    return map;
}

void unloadMap(int** map, int mapWidth, int mapHeight)
{
    for (int i = 0; i < mapHeight; i++)
    {
        delete[] map[i];
    }

    delete[] map;
}
