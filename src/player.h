#ifndef PLAYER_H
#define PLAYER_H

#include "vector2d.h"

class Player
{
public:
    Vector2d dir;
    Vector2d pos;
    Vector2d cameraPlane;
    Vector2i mapPos;

    Player();
    Player(Vector2d dirV, Vector2d posV);

    void rotate(double degrees);
};

#endif