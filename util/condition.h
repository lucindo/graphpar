//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#if !defined(condition_H)
#define condition_H

#include "mutex.h"
#include "os.h"

class Condition
{
    private:
        Mutex& mutex_;
        os::cond_t condition_;
        
    public:
        Condition(Mutex& mutex);
        ~Condition();
        
        // waits to the condition
        int wait();
        // signilize one thread for the condition
        int signal();
        // signilize all thread for the condition
        int broadcast();
        // destroys the condition variable
        int destroy();
};

#endif // condition_H
