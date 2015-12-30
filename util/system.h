//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

// This file must be generated automatically

// to g++ default defines: g++ -dM -E -xc /dev/null

#if !defined(system_H)
#define system_H


#if defined(__gnu_linux__) || defined(__APPLE__)

#define IN_LINUX
#include <pthread.h>

#if defined(__APPLE__)
#include <sched.h>
#define pthread_yield sched_yield
#endif

#else

#error "Ony linux with pthreads supported for now"

#endif

#endif // system_H
