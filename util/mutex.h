//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#if !defined(mutex_H)
#define mutex_H

#include "lock.h"

#include "os.h"

// Implements a mutex
class Mutex : public virtual Lock
{
    private:
        os::mutex_t mutex_;
        
    public:
        Mutex();
        
        ~Mutex();
        
	// aquires the lock
        void aquire();
        
        // release the lock
        void release();
        
        os::mutex_t& lock()
        {
            return mutex_;
        }
};

#endif // mutex_H
