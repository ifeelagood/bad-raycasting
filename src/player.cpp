#include <math.h>
#include "player.h"


Player::Player() : dir(Vector2d(0,0)), pos(Vector2d(0,0)) {};

Player::Player(Vector2d dirV, Vector2d posV)
{
    dir = dirV; pos = posV;
}

void Player::rotate(double deg) { dir.rotateByDeg(deg); cameraPlane.rotateByDeg(deg); }
