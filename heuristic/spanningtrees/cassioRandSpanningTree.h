//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#if !defined(cassioRandSpanningTree_H)
#define cassioRandSpanningTree_H

#include "cassioRandSpanningTree.h"
#include "heuristicCommon.h"
#include "partition.h"
#include "graph.h"

Partition cassioRandSpanningTreeHeuristic(const Graph& graph, unsigned clusters, double time = 0);

#endif // cassioRandSpanningTree_H
