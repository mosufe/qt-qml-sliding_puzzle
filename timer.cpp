#include "timer.h"

Timer::Timer()
{
    startTimer();
}

void Timer::startTimer()
{
    time.start();
}

QTime Timer::getQTimer()
{
    return time;
}

int Timer::getSeconds()
{
    return time.restart()/1000;
}
