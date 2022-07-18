#include "map.h"
#include <string>

int** loadMap(std::string path, int &MAP_WIDTH, int &MAP_HEIGHT)
{
    std::ifstream mapFile;
    mapFile.open(path);

    std::string line;

    // determine dimesions
    int mapX, mapY;

    mapY = 0;
    if ( mapFile.is_open() )
    {
        char c;
        while (std::getline(mapFile, line))
        {
            mapY++;
            mapX = line.length();
        }
    }

    mapFile.close();

    MAP_WIDTH = mapX;
    MAP_HEIGHT = mapY;

    // create map array
    int** map = new int*[MAP_HEIGHT];
    for (int i = 0; i < MAP_HEIGHT; i++)
    {
        map[i] = new int[MAP_WIDTH];
    }

    // populate map

    mapFile.open(path);


    int x,y;
    y = 0;
    if ( mapFile.is_open() )
    {
        while (std::getline(mapFile, line))
        {

            for (x = 0; x < MAP_WIDTH; x++)
            {
                map[y][x] = (int) line[x] - 48;
            }
            y++;
        }

    }

    return map;
}

void deleteMap(int** map, int MAP_WIDTH, int MAP_HEIGHT)
{
    for (int i = 0; i < MAP_HEIGHT; i++)
    {
        delete[] map[i];
    }

    delete[] map;
}
