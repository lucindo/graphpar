//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#if !defined(os_H)
#define os_H

#include "system.h"

// All OS dependent code goes here. If don't, it should :)
// OS dependencies:
//   - mutexes
//   - condition variables
//   - threads

namespace os
{
#if defined(IN_LINUX)
    typedef pthread_mutex_t mutex_t;
    typedef pthread_cond_t  cond_t;    
#else
#error "Only Linux is supported now"
#endif

    int mutex_init(mutex_t * mutex);
    int mutex_destroy(mutex_t * mutex);
    int mutex_lock(mutex_t * mutex);
    int mutex_unlock(mutex_t * mutex);
    
    int cond_init(cond_t * cond);
    int cond_destroy(cond_t * cond);
    int cond_wait(cond_t * cond, mutex_t * mutex);
    int cond_signal(cond_t * cond);
    int cond_broadcast(cond_t * cond);
    
    void thread_yield();
};

#endif // os_H

