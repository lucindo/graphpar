//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#if !defined(Random_H)
#define Random_H

class RandomSeed
{
    private:
        static RandomSeed * instance_;
        bool seedStarted_;
        
        RandomSeed();
        
    public:
        static RandomSeed * instance();
        
        void restartSeed();
        void startSeed();
};

class Random
{
    public:
        Random(bool = false);
        
        int rand(int min, int max);
        
        int rand(int max);
};

#endif // Random_H
