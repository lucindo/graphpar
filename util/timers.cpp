//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#include "timers.h"
#include <cassert>

// starts the timer
void Timer::start()
{
    assert(gettimeofday(&(this->startTime_), &(this->tmzone_)) == 0);
}

// stops the timer
void Timer::stop()
{
    assert(gettimeofday(&(this->stopTime_), &(this->tmzone_)) == 0);
}

// calculate elapsed time in seconds
// don't check if either start() or stop() had been called
double Timer::getElapsedTime() const
{
    time_t      tv_sec  = this->stopTime_.tv_sec - this->startTime_.tv_sec;
    suseconds_t tv_usec = this->stopTime_.tv_usec - this->startTime_.tv_usec;
    
    return tv_sec + ((double) tv_usec) / ONE_SECOND_IN_USECS;
}

double Timer::getTime()
{
    struct timeval now;
    assert(gettimeofday(&now, &(this->tmzone_)) == 0);

    time_t      tv_sec  = now.tv_sec - this->startTime_.tv_sec;
    suseconds_t tv_usec = now.tv_usec - this->startTime_.tv_usec;
    
    return tv_sec + ((double) tv_usec) / ONE_SECOND_IN_USECS;
}

ScopedTimer::ScopedTimer(const char * message, LogPriority priority) : message_(message), priority_(priority)
{
    this->timer_.start();
}

ScopedTimer::~ScopedTimer()
{
    this->timer_.stop();
    log(this->priority_, "%s took %5.6f secs", this->message_, this->timer_.getElapsedTime());
}
