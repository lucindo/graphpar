//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#if !defined(Lock_H)
#define Lock_H

// Lock interface
class Lock
{
    public:
        virtual ~Lock()
        {
        }
        
        // aquires the lock
        virtual void aquire() = 0;
        
        // release the lock
        virtual void release() = 0;
};

#endif // Lock_H
