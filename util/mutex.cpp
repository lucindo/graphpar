//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#include "mutex.h"

Mutex::Mutex()
{
    os::mutex_init(&this->mutex_);
}

Mutex::~Mutex()
{
    os::mutex_destroy(&this->mutex_);
}

void Mutex::aquire()
{
    os::mutex_lock(&this->mutex_);
}

void Mutex::release()
{
    os::mutex_unlock(&this->mutex_);
}
