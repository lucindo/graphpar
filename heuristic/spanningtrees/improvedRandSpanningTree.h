//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#if !defined(improvedRandSpanningTree_H)
#define improvedRandSpanningTree_H

#include "graph.h"
#include "partition.h"

Partition improvedRandSpanningTreeHeuristic(const Graph& graph, unsigned clusters, bool tryRand, double time = 0);

#endif // improvedRandSpanningTree_H
