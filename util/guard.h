//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#if !defined(Guard_H)
#define Guard_H

#include "lock.h"

// Scoped Lock Guard pattern
class Guard
{
    private:
        Lock& lock_;
    
    public:
        Guard(Lock& lock);
        
        ~Guard();
};

#endif // Guard_H
