#include "vector2d.h"
#include <math.h>


double Vector2d::distance(Vector2d b) { return std::sqrt((b.x - x)*(b.x - x) + (b.y - y)*(b.y - y)); }


double deg2rad(double deg) { double rad = deg * (PI/180); return rad; }
double rad2deg(double rad) { double deg = rad * (180/PI); return deg; }

void fixRad(double &rad)
{
    rad -= 0.000001; // subtract for precision
    if (rad < -PI) { rad = PI; }
    if (rad > PI) { rad = -PI; }
}

void fixDeg(double &deg)
{
    deg -= 0.000001;

    if (deg < -180) { deg = 180; }
    if (deg > 180) { deg = -180; }
}


double Vector2d::vec2rad()
{
    double rad = atan2(x,y);
    return rad;
}


double Vector2d::vec2deg()
{
    double rad = atan2(x,y);
    double deg = rad2deg(rad);
    return deg;
}

void Vector2d::rotateByRad(double rad)
{
    double alpha;
    double x1, y1, x2, y2;
    x1 = x; y1 = y;

    alpha = rad * -1.0f;

    x2 = (x1 * cos(alpha)) - (y1 * sin(alpha));
    y2 = (x1 * sin(alpha)) + (y1 * cos(alpha));

    x = x2; y = y2;
}

void Vector2d::rotateByDeg(double deg)
{
    double alpha;
    double x1, y1, x2, y2;
    x1 = x; y1 = y;

    alpha = deg2rad(deg) * -1.0f;

    x2 = (x1 * cos(alpha)) - (y1 * sin(alpha));
    y2 = (x1 * sin(alpha)) + (y1 * cos(alpha));

    x = x2; y = y2;
}


double Vector2d::abs()
{
    return sqrt(x*x+y*y);
}

Vector2d translate(Vector2d position, Vector2d direction, double distance)
{
    Vector2d v;

    v.x = position.x + (direction.x * distance);
    v.y = position.y + (direction.y * distance);

    return v;
}


// #############################
// #########OPERATORS###########
// #############################

// direct assignment vector
Vector2d Vector2d::operator+(Vector2d const &a) {
	return Vector2d { x + a.x, y + a.y };
}

Vector2d Vector2d::operator-(Vector2d const &a) {
	return Vector2d { x - a.x, y - a.y };
}

Vector2d Vector2d::operator*(Vector2d const &a) {
	return Vector2d { x * a.x, y * a.y };
}

Vector2d Vector2d::operator/(Vector2d const &a) {
	return Vector2d { x / a.x, y / a.y };
}

// direct assignment double
Vector2d Vector2d::operator+(const double &a) {
	return Vector2d { x + a, y + a };
}

Vector2d Vector2d::operator-(const double &a) {
	return Vector2d { x - a, y - a };
}

Vector2d Vector2d::operator*(const double &a) {
	return Vector2d { x * a, y * a };
}

Vector2d Vector2d::operator/(const double &a) {
	return Vector2d { x / a, y / a };
}


// operator assignment vector
void Vector2d::operator+=(Vector2d const &a) {
	x += a.x; y += a.y;
}

void Vector2d::operator-=(Vector2d const &a) {
	x -= a.x; y -= a.y;
}

void Vector2d::operator*=(Vector2d const &a) {
	x *= a.x; y *= a.y;
}

void Vector2d::operator/=(Vector2d const &a) {
	x /= a.x; y /= a.y;
}


// operator assignment double
void Vector2d::operator+=(const double &a) {
	x += a; y += a;
}

void Vector2d::operator-=(const double &a) {
	x -= a; y -= a;
}

void Vector2d::operator*=(const double &a) {
	x *= a; y *= a;
}

void Vector2d::operator/=(const double &a) {
	x /= a; y /= a;
}
