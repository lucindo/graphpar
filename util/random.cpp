//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#include "random.h"
#include <cstdlib>
#include <time.h>

RandomSeed * RandomSeed::instance_ = 0;

// Returns the instance of Logger utility (not thread-safe)
RandomSeed * RandomSeed::instance()
{
    if (instance_ == 0)
    {
        instance_ = new RandomSeed();
    }
    
    return instance_;
}

RandomSeed::RandomSeed()
{
}

void RandomSeed::startSeed()
{
    if (not seedStarted_)
    {
        srand(time(0));
        seedStarted_ = true;
    }
}

void RandomSeed::restartSeed()
{
    srand(time(0));
    seedStarted_ = true;
}


Random::Random(bool restartSeed)
{
    if (restartSeed)
    {
        RandomSeed::instance()->restartSeed();
    }
    else
    {
        RandomSeed::instance()->startSeed();
    }
}

// From man 3 rand:
// "If  you  want to generate a random integer between 1 and 10, you should always do it by
//  using high-order bits, as in
//      j=1+(int) (10.0*rand()/(RAND_MAX+1.0));
//  and never by anything resembling
//      j=1+(rand() % 10);
//  (which uses lower-order bits)."

int Random::rand(int min, int max)
{
    double dmin = static_cast<double>(min);
    double dmax = static_cast<double>(max);
    
    return min + (int) (dmax * ::rand() / (RAND_MAX + dmin));
}

int Random::rand(int max)
{
    return this->rand(0, max);
}

