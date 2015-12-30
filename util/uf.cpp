//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#include "uf.h"
#include <cassert>

UF::UF(unsigned nElem)
    : marks_(nElem)
{
    for (unsigned e = 0; e < marks_.size(); e++)
    {
        marks_[e] = e;
    }
}

unsigned UF::findElement(unsigned elem) const
{
    assert(elem < marks_.size());
    
    return marks_[elem];
}

void UF::unionElements(unsigned u, unsigned v)
{
    assert(u < marks_.size());
    assert(v < marks_.size());
    
    // naive union & find
    unsigned oldVmark = marks_[v];
    for (unsigned m = 0; m < marks_.size(); ++m)
    {
	if (marks_[m] == oldVmark)
	{
	    marks_[m] = marks_[u];
	}
    }
}

