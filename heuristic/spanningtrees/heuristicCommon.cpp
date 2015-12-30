//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#include "heuristicCommon.h"

bool isBetter(const Partition& p1, const Partition& p2)
{
    assert(p1.nComponents() == p2.nComponents());
    
    // can return (p1.heaviestWeight() - p1.lightestWeight()) < (p2.heaviestWeight() - p2.lightestWeight())
    return p1.lightestWeight() > p2.lightestWeight();
}
