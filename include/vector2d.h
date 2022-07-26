#pragma once

#define PI 3.141592653589

class Vector2d
{
public:
    double x,y;

    // class constructors
    Vector2d() : x(0.0f), y(0) {};
    Vector2d(double vx, double vy) : x(vx), y(vy) {};

    // get distance to other vector
    double distance(Vector2d b);

    // return vector as degrees or radians
    double vec2deg();
    double vec2rad();

    // rotation matricies
    void rotateByRad(double rad);
    void rotateByDeg(double deg);

    // translate points by vector
    void translate(Vector2d &coord);

    // get absolute value of vector
    double abs();

    // direct assignment vector
    Vector2d operator+(Vector2d const &a);
	Vector2d operator-(Vector2d const &a);
	Vector2d operator*(Vector2d const &a);
    Vector2d operator/(Vector2d const &a);

    // direct assignment double
    void operator+=(Vector2d const &a);
    void operator-=(Vector2d const &a);
	void operator*=(Vector2d const &a);
	void operator/=(Vector2d const &a);

    // operator assignment vector
    Vector2d operator+(const double &a);
    Vector2d operator-(const double &a);
    Vector2d operator*(const double &a);
    Vector2d operator/(const double &a);

    // operator assignment double
    void operator+=(const double &a);
    void operator-=(const double &a);
	void operator*=(const double &a);
	void operator/=(const double &a);


};

class Vector2i
{
public:
    int x, y;

    Vector2i() : x(0), y(0) {};
    Vector2i(int vx, int vy) : x(vx), y(vy) {};
};


double deg2rad(double deg);
double rad2deg(double rad);

void fixRad(double &rad);
void fixDeg(double &deg);


Vector2d translate(Vector2d position, Vector2d direction, double distance);


template <class T>
class Vector2
{
public:
    T x,y;
    Vector2<T>();
    Vector2(T _x, T _y) : x(_x), y(_y) {};
};