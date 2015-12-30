//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#include "os.h"

// All OS dependent code goes here. If don't, it should :)

namespace os
{
    int mutex_init(mutex_t * mutex)
    {
        #if defined(IN_LINUX)
        return pthread_mutex_init(mutex, NULL);
        #endif
    }
    
    int mutex_destroy(mutex_t * mutex)
    {
        #if defined(IN_LINUX)
        return pthread_mutex_destroy(mutex);
        #endif
    }
    
    int mutex_lock(mutex_t * mutex)
    {
        #if defined(IN_LINUX)
        return pthread_mutex_lock(mutex);
        #endif
    }
    
    int mutex_unlock(mutex_t * mutex)
    {
        #if defined(IN_LINUX)
        return pthread_mutex_unlock(mutex);
        #endif
    }
    
    int cond_init(cond_t * cond)
    {
        #if defined(IN_LINUX)
        return pthread_cond_init(cond, NULL);
        #endif
    }
    
    int cond_destroy(cond_t * cond)
    {
        #if defined(IN_LINUX)
        return pthread_cond_destroy(cond);
        #endif    
    }
    
    int cond_wait(cond_t * cond, mutex_t * mutex)
    {
        #if defined(IN_LINUX)
        return pthread_cond_wait(cond, mutex);
        #endif    
    }
    
    int cond_signal(cond_t * cond)
    {
        #if defined(IN_LINUX)
        return pthread_cond_signal(cond);
        #endif    
    }
    
    int cond_broadcast(cond_t * cond)
    {
        #if defined(IN_LINUX)
        return pthread_cond_broadcast(cond);
        #endif    
    }
    
    void thread_yield()
    {
        #if defined(IN_LINUX)
        pthread_yield();
        #endif
    }
};
