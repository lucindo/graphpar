//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#if !defined(improvedHeuristicCommon_H)
#define improvedHeuristicCommon_H

#include "graph.h"
#include "partition.h"

// Helper function used on improvedRandSpanningTreeHeuristic
// Takes the original graph, a spanning tree of it and a partition of it's vertex set
// and try to change the spanning in a way that one light partition component will be
// connected to one heavy (connection non-existent before).
// When doSort == false trys to connect two random unconnected components on tree
// If the change is not possible returns false
bool adjustTree(const Graph& graph, Graph& tree, Partition& partition, bool doSort);


#endif // improvedHeuristicCommon_H
