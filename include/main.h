#pragma once

#include <vector>
#include "config.h"

typedef struct { bool w,a,s,d,m,up,down,left,right,tab,esc,lshift; } ButtonKeys;

Config config("config.ini");

int mapWidth, mapHeight;
int texWidth, texHeight;

bool done = false;

std::vector<std::vector<uint32_t>> texture;

Timer timer;
ButtonKeys Keys;
Player player;

int level = 1;

int** map;