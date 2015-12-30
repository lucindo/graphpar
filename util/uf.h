//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#if !defined(uf_H)
#define uf_H

// Naive Union & Find implementation
// TODO: a real fast one

#include <vector>

class UF
{
    private:
        std::vector<unsigned> marks_;
    
    public:
        UF(unsigned nElem);
        
        unsigned findElement(unsigned elem) const;
        void unionElements(unsigned u, unsigned v);
};

#endif // uf_H
