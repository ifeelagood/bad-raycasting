#include "timer.h"

// #include <numeric>

void fTimer::update(double ticks)
{
    old = current;
    current = ticks;
    frameTime = (current - old) / 1000.0f;
    fps = 1.0f / frameTime;
    // fpsBuffer.push_back(fps);
}

// double Timer::getAverageFPS()
// {
//     return std::reduce(fpsBuffer.begin(), fpsBuffer.end()) / fpsBufferLength;
// }