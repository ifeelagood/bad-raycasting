#include "timer.h"

Timer::Timer() {};


void Timer::update(double ticks)
{
    old = current;
    current = ticks;
    frameTime = (current - old) / 1000.0f;
    fps = 1.0f / frameTime;
}