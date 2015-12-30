//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#include "mixedImprovedRandSpanningTree.h"
#include "improvedHeuristicCommon.h"
#include "heuristicCommon.h"

#include "random.h"
#include "log.h"
#include "timers.h"

// Mixied Improved Heuristic:
// Do the following |V(G)| times
// T = random spanning tree of G
// P = solution of Perl and Schach max-min algorithm for T
// while impove solution do
//   G = T with a conection between the lightest and heaviest components
//   T = remove cicle from T
//   P = solution of Perl and Schach max-min algorithm for T
// return the best solution found
Partition mixedImprovedRandSpanningTreeHeuristic(const Graph& graph, unsigned clusters, unsigned tries, double time)
{
    Partition best;
    int improved = -1;
    
    Timer timer;
    timer.start();
    
    for (unsigned t = 0; t < tries; ++t)
    {
	if (time > 0.0)
        {
            if (timer.getTime() > time)
            {
                break;
            }
        }

	std::vector<PartitionComponent> components;
	
	Graph stree = graph.getRandomSpanningTree();
        
	Tree ptree(graph.nVertices() + 1);
	getTreeFromRandomTree(stree, &ptree, false);
    
	partitionByPerlSchach(ptree, clusters, components);
	
	assert(clusters == components.size());
	
	Partition initial(components);
	if (improved == -1)
	{
	    improved++;
	    best = initial;	   
	}
	else if (isBetter(initial, best))
	{
	    improved++;
	    best = initial;
	}
	
	bool found = true;
	unsigned adjusts = 0;	
	while (found and adjusts < graph.nVertices())
	{
	    found = false;
	    adjusts++;
	    
	    Partition current;
	    if (adjustTree(graph, stree, best, true) == true)
	    {
		std::vector<PartitionComponent> comps;
		Tree ntree(graph.nVertices() + 1);
		getTreeFromRandomTree(stree, &ntree, false);
		
		partitionByPerlSchach(ntree, clusters, comps);
		current = comps;
		
		if (isBetter(current, best))
		{
		    improved++;
		    best = current;
		    found = true;
		}
	    }
	}
    }
        
    // adjust results
    std::vector<PartitionComponent> components;
    components.resize(best.nComponents());
    for (unsigned p = 0; p < best.nComponents(); ++p) 
    {
	components[p].weight = best.component(p).weight;
	components[p].vertices.clear();
	for (std::set<unsigned>::const_iterator v = best.component(p).vertices.begin(); v != best.component(p).vertices.end(); v++)
	   if (*v != 0) components[p].addVertex((*v) - 1);
    }
    
    log(LG_DEBUG, "mixedImprovedRandSpanningTreeHeuristic impoved the original solution %u times", improved);
    return components;
}
