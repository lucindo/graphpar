//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#if !defined(randSpaningTree_H)
#define randSpaningTree_H

#include "graph.h"
#include "partition.h"

Partition randSpanningTreeHeuristic(const Graph& graph, unsigned clusters, unsigned tries, Graph& tree, double time = 0, int giveup = 0);

#endif // randSpaningTree_H
