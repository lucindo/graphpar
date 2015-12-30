//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#if !defined(mixedImprovedRandSpanningTree_H)
#define mixedImprovedRandSpanningTree_H

#include "graph.h"
#include "partition.h"

Partition mixedImprovedRandSpanningTreeHeuristic(const Graph& graph, unsigned clusters, unsigned tries, double time = 0);

#endif // mixedImprovedRandSpanningTree_H
