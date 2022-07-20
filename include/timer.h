#pragma once

#include <vector>

class Timer
{
private:
    const int fpsBufferLength = 1000; // FPS/10seconds

public:
    double old = 0.0;
    double current = 0.0;

    double frameTime = 0.0;
    double fps = 0.0;

    std::vector<double> fpsBuffer;;

    Timer() : fpsBuffer(fpsBufferLength) {};
    void update(double ticks);
    double getAverageFPS();
};