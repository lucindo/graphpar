//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#include "guard.h"

Guard::Guard(Lock& lock)
    : lock_(lock)
{
    lock_.aquire();    
}

Guard::~Guard()
{
    lock_.release();
}
