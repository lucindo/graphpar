//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#if !defined(generators_H)
#define generators_H

#include "graph.h"
#include "tree.h"

Graph generateRandomTree(unsigned nVertices, unsigned totalWeight);

Graph generateBalancedGraph(unsigned nVertices, unsigned nComponents, unsigned totalWeight, unsigned density);

Graph generateRandomGraph(unsigned nVertices, unsigned density, unsigned maxVertexWeight = 3);

unsigned getTotalEdgesFromDensity(unsigned nVertices, unsigned density);

#endif // generators_H
