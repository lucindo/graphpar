//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#include "condition.h"
#include <errno.h>

Condition::Condition(Mutex& mutex)
    : mutex_(mutex)
{
    os::cond_init(&this->condition_);
}

Condition::~Condition()
{
    this->destroy();
}

int Condition::destroy()
{
    int retval = 0;
    while ((retval = os::cond_destroy(&this->condition_)) == -1 
            && errno == EBUSY)
    {
        this->broadcast();
        os::thread_yield();
    }
    
    return retval;
}

int Condition::wait()
{
    return os::cond_wait(&this->condition_, &this->mutex_.lock());
}

int Condition::signal()
{
    return os::cond_signal(&this->condition_);
}

int Condition::broadcast()
{
    return os::cond_broadcast(&this->condition_);
}
