//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#if !defined(timers_H)
#define timers_H

// This classes and functions are system dependend
// It should compile on every Unix (posix, bsd, svr4)

#include <sys/time.h>
#include <time.h>

#include "log.h"

#define ONE_SECOND_IN_USECS 1000000L

class Timer
{
    private:
        struct timezone tmzone_;
        struct timeval startTime_;
        struct timeval stopTime_;
        
    public:
        Timer()
        {
        }
        
        void start();
        void stop();
        
        double getElapsedTime() const;
        
        double getTime();
};

class ScopedTimer
{
    private:
        const char * message_;
        LogPriority priority_;
        Timer timer_;
        
    public:
        ScopedTimer(const char * message, LogPriority priority = LG_INFO);
        ~ScopedTimer();    
};

#endif // timers_H
