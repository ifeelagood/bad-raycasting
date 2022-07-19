#pragma once

class Timer
{
public:
    double old = 0.0;
    double current = 0.0;

    double frameTime = 0.0;
    double fps = 0.0;

    void update(double ticks);

    Timer();

};