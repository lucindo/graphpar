//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#if !defined(messageQueue_H)
#define messageQueue_H

#include <queue>
#include "messageBlock_T.h"
#include "mutex.h"
#include "condition.h"
#include "guard.h"

// Sychronized message queue

template <class data_t>
class MessageQueue
{
    private:
        Mutex mutex_;
        Condition notemptycond_;
        std::queue<MessageBlock<data_t> * > queue_;

    public:
        MessageQueue() : notemptycond_(mutex_)
        {
        }

        ~MessageQueue()
        {
            Guard guard(mutex_);
            while (not queue.empty())
            {
                MessageBlock<data_t> * data = queue.front();
                data->release();
                queue.pop();
            }
        }

        void enqueue(MessageBlock<data_t> * mb)
        {
            {
                Guard guard(mutex_);
                queue.push(mb);
            }
            notemptycond_.signal();
        }

        MessageBlock<data_t> * dequeue()
        {
            if (queue.empty())
                notemptycond_.wait();

            Guard guard(mutex_);
            MessageBlock<data_t> * data = queue.front();
            queue.pop();
            return data;
        }

        unsigned size()
        {
            Guard guard(mutex_);
            return queue.size();
        }
};

#endif // messageQueue_H
