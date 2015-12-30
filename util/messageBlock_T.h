//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#if !defined(messageBlock_H)
#define messageBlock_H

// Implements a simple MessageBlock of data_t
// You shoud allocate data_t on heap to instatiate
// a MessageBlock. The message block owns the data
// encapsulated and you must call release() in order
// to avoid leak memorys
// All MessageBlocks MUST be alloced on heap

template <class data_t>
class MessageBlock
{
    public:
        enum MessageType
        {
            M_BEGIN,
            M_DATA,
            M_LAST
        };

    private:
        data_t * data_;
        MessageType type_;
        bool shouldRelease_;

    public:
        MessageBlock(data_t * data, MessageType type = M_DATA)
            : data_(data), type_(type), shouldRelease_(data_ != 0)
        {
        }

        data_t * data()
        {
            return data_;
        }

        MessageType type()
        {
            return type_;
        }

        void release()
        {
            if (shouldRelease_)
            {
                delete data_;
            }
            delete this;
        }
};

#endif // messageBlock_H
