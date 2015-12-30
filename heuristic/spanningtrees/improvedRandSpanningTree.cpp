//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#include "improvedRandSpanningTree.h"
#include "improvedHeuristicCommon.h"
#include "heuristicCommon.h"

#include "generators.h"
#include "log.h"
#include "random.h"
#include "timers.h"

#include <queue>
#include <vector>
#include <stack>

// Improved Heuristic:
// T = random spanning tree of G
// P = solution of Perl and Schach max-min algorithm for T
// while impove solution do
//   G = T with a conection between the lightest and heaviest components
//   T = remove cicle from T
//   P = solution of Perl and Schach max-min algorithm for T
Partition improvedRandSpanningTreeHeuristic(const Graph& graph, unsigned clusters, bool tryRand, double time)
{
    Random random;
    std::vector<PartitionComponent> components;
    
    Timer timer;
    timer.start();
        
    Graph stree = graph.getRandomSpanningTree();
	    
    Tree ptree(graph.nVertices() + 1);
    getTreeFromRandomTree(stree, &ptree, false);

    partitionByPerlSchach(ptree, clusters, components);
    
    assert(clusters == components.size());
    
    Partition best(components);
    Partition * lastComputed = &best;
    bool found = true;
    
    int improved = 0;
    unsigned adjusts = 0;
    
    while (found and adjusts < graph.nVertices())
    {
	if (time > 0.0)
        {
            if (timer.getTime() > time)
            {
                break;
            }
        }

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
	    lastComputed = &current;
	    
            if (isBetter(current, best))
            {
                improved++;
                best = current;
                found = true;
            }
        }
        if (not found and tryRand)
        {
            // try some random adjustments
            unsigned totalTries = random.rand(1, clusters);
            for (unsigned t = 0; t < totalTries; t++)
            {
                adjusts++;
                if (adjustTree(graph, stree, *lastComputed, false) == true)
                {
		    std::vector<PartitionComponent> comps;
		    Tree ntree(graph.nVertices() + 1);
		    getTreeFromRandomTree(stree, &ntree, false);
		    
		    partitionByPerlSchach(ntree, clusters, comps);
		    current = comps;
		    lastComputed = &current;
		    
		    if (isBetter(current, best))
		    {
			improved++;
			best = current;
			found = true;
			break;
		    }                    
                }
            }
        }
    }
    
    // adjust results
    components.resize(best.nComponents());
    for (unsigned p = 0; p < best.nComponents(); ++p) 
    {
	components[p].weight = best.component(p).weight;
	components[p].vertices.clear();
	for (std::set<unsigned>::const_iterator v = best.component(p).vertices.begin(); v != best.component(p).vertices.end(); v++)
	   if (*v != 0) components[p].addVertex((*v) - 1);
    }
    
    log(LG_DEBUG, "improvedRandSpanningTreeHeuristic impoved the original solution %u times", improved);
    return components;
}

