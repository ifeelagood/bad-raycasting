#pragma once

#include "vector2d.h"

class Player
{
    public:
    Vector2d direction;
    Vector2d position;
    Vector2d cameraPlane;

    Player();
    Player(Vector2d playerDirection, Vector2d playerPosition, Vector2d playerCameraPlane);

    void rotate(double degrees);
    void cameraFix();
};