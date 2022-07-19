#include "player.h"


Player::Player() : direction(Vector2d(0,0)), position(Vector2d(0,0)), cameraPlane(Vector2d(0,0)) {};

Player::Player(Vector2d playerDirection, Vector2d playerPosition, Vector2d playerCameraPlane)
{
    direction = playerDirection;
    position = playerPosition;
    cameraPlane = playerCameraPlane;
}

void Player::rotate(double deg) { direction.rotateByDeg(deg); cameraPlane.rotateByDeg(deg); }

void Player::cameraFix() { cameraPlane = Vector2d(direction.y, -direction.x); }