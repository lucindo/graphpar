//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#include "randSpanningTree.h"
#include "generators.h"
#include "log.h"
#include "timers.h"

// Simplest heuristic:
// do <tries> times:
//   Get a random spanning tree T of Graph. 
//   Apply the Perl and Schach max-min algorithm for T
// return the best solution found.
Partition randSpanningTreeHeuristic(const Graph& graph, unsigned clusters, unsigned tries, Graph& tree, double time, int giveup)
{
    unsigned best = std::numeric_limits<unsigned>::min();
    std::vector<PartitionComponent> components;
    
    int attemptsNotImproving = 0;
    unsigned t = 0;
    Timer timer;
    timer.start();
    for (;;)
    {
        if (time > 0.0)
        {
            if (timer.getTime() > time)
            {
                break;
            }
        }
        else if (t > tries) break;
        std::vector<PartitionComponent> comps;
        
	Graph stree = graph.getRandomSpanningTree();
	    
	Tree ptree(graph.nVertices() + 1);
	getTreeFromRandomTree(stree, &ptree, false);
    
	partitionByPerlSchach(ptree, clusters, comps);
        
        assert(clusters == comps.size());
        
        unsigned lightest = std::numeric_limits<unsigned>::max();
        for (unsigned c = 0; c < comps.size(); ++c)
        {
            if (comps[c].weight < lightest)
            {
                lightest = comps[c].weight;
            }
        }
        if (lightest > best)
        {
            best = lightest;
            attemptsNotImproving = 0;
            components.resize(comps.size());
            tree = stree;
	    for (unsigned p = 0; p < comps.size(); ++p) 
	    {
	       components[p].weight = comps[p].weight;
	       components[p].vertices.clear();
	       for (std::set<unsigned>::const_iterator v = comps[p].vertices.begin(); 
	            v != comps[p].vertices.end(); v++)
	       {
	           if (*v != 0)
	           {
	               components[p].addVertex((*v) - 1);
	           }  
	       }
	    }
        }
        else attemptsNotImproving++;
        
        if (giveup and attemptsNotImproving >= giveup)
        {
            log(LG_DEBUG, "Giving up after %d tries with no improvement", attemptsNotImproving);
            break;
        }
        ++t;
    }
    
    return components;
}
