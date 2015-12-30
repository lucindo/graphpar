//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#if !defined(task_H)
#define task_H

#include "os.h"
#include "messageQueue_T.h"

template <class data_t>
class Task
{
    private:
        volatile MessageQueue<data_t> queue;

    public:

        int activate(int nThreads = 1)
        {
        }
};

#endif // task_H
