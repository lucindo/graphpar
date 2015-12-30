//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#include "cassioRandSpanningTree.h"

#include "log.h"
#include "random.h"
#include "timers.h"

#include <stack>

// Heuristic due to Cassio Polpo de Campos <cassio@ime.usp.br>
// T = random spanning tree of graph
// best = solution of Perl and Schach max-min algorithm for T
// do while improve the best solution
//     for each edge e not in T do
//         add e to T
//         for each edge l on fundamental cicle of T and differ than e do
//             remove l from T
//             current = solution of Perl and Schach max-min algorithm for T
//             if current is better then best
//                 best = current
//                 goto while
//             add l back to T
//        remove e from T
#if 0
Partition cassioRandSpanningTreeHeuristic(const Graph& graph, unsigned clusters)
{
    std::vector<PartitionComponent> components;
    
    graph.updatedEdges();
    
    Graph stree = graph.getRandomSpanningTree(); // the edges list is built
	    
    Tree ptree(graph.nVertices() + 1);
    getTreeFromRandomTree(stree, &ptree, false);

    partitionByPerlSchach(ptree, clusters, components);
    
    assert(clusters == components.size());
    
    Partition best(components);
    bool found = true;
    unsigned improved = 0;
    
    while (found)
    {
        start:
        found = false;
        
        for (unsigned e = 0; e < graph.nEdges(); e++)
            if (not stree.isConnected(graph.edges_[e].u, graph.edges_[e].v))
            {
                unsigned u = graph.edges_[e].u;
                unsigned v = graph.edges_[e].v;
                
                log(LG_DEBUG, "adding edge (%u, %u) to tree", u, v);
                stree.addConnection(u, v);
                
                //
                // Seach for edges on fundamental circle start
                //
                std::vector<bool> visited(stree.nVertices(), false);
                std::stack<unsigned> stack;
                
                visited[u] = visited[v] = true;
		stack.push(v);
		while (not stack.empty())
		{
		    dfs_start:
		    unsigned w = stack.top();
		    
		    // see if we can go down
		    bool shouldStop = false;
		    const std::set<unsigned>& adjs = stree.adjs_[w];
		    for (std::set<unsigned>::const_iterator x = adjs.begin(); x != adjs.end(); x++)
			if (*x != u and not visited[*x])
			{
			    visited[*x] = true;
			    stack.push(*x);
			    goto dfs_start;
			}
			else if (stack.size() > 1 and *x == u) // found the circuit!
			{
			    stack.push(*x);
			    shouldStop = true;
			    break;
			}
		    
		    if (shouldStop) break;
		    stack.pop();
		}
		
		assert(not stack.empty());
                //
                // Seach for edges on fundamental circle end
                //
                std::vector<unsigned> cirleVertices;
		while (not stack.empty())
		{
		    unsigned e = stack.top();
		    cirleVertices.push_back(e);
		    stack.pop();
		}
		
		for (unsigned s = 0; s < cirleVertices.size() - 1; s++)
		{
		    log(LG_DEBUG, "circuit edge found (%u, %u)", cirleVertices[s], cirleVertices[s+1]);
		    assert(stree.isConnected(cirleVertices[s], cirleVertices[s+1]));
		    
		    stree.removeConnection(cirleVertices[s], cirleVertices[s+1]);
		    
                    std::vector<PartitionComponent> comps;
   
		    Tree ntree(stree.nVertices() + 1);
		    getTreeFromRandomTree(stree, &ntree, false);
		
		    partitionByPerlSchach(ntree, clusters, comps);
		    
		    assert(clusters == comps.size());
		    
		    Partition current(comps);

		    if (isBetter(current, best))
		    {
			improved++;
			best = current;
			found = true;
			goto start;
		    }
		    		    
		    stree.addConnection(cirleVertices[s], cirleVertices[s+1]);
		}

                
                // step back
                log(LG_DEBUG, "removing edge (%u, %u) to tree", u, v);
                stree.removeConnection(u, v);
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
    
    log(LG_DEBUG, "cassioRandSpanningTreeHeuristic impoved the original solution %u times", improved);
    return components;
}
#else
Partition cassioRandSpanningTreeHeuristic(const Graph& graph, unsigned clusters, double time)
{
    std::vector<PartitionComponent> components;
    
    Timer timer;
    timer.start();
    
    graph.updatedEdges();
    
    Graph stree = graph.getRandomSpanningTree(); // the edges list is built
	    
    Tree ptree(graph.nVertices() + 1);
    getTreeFromRandomTree(stree, &ptree, false);

    partitionByPerlSchach(ptree, clusters, components);
    
    assert(clusters == components.size());
    
    Partition best(components);
    unsigned improved = 0;
    
    for (unsigned e = 0; e < graph.nEdges(); e++)
    {
        if (time > 0.0)
        {
            if (timer.getTime() > time)
            {
                break;
            }
        }
	if (not stree.isConnected(graph.edges_[e].u, graph.edges_[e].v))
	{
	    unsigned u = graph.edges_[e].u;
	    unsigned v = graph.edges_[e].v;
	    
	    log(LG_DEBUG, "adding edge (%u, %u) to tree", u, v);
	    stree.addConnection(u, v);
	    
	    //
	    // Seach for edges on fundamental circle start
	    //
	    std::vector<bool> visited(stree.nVertices(), false);
	    std::stack<unsigned> stack;
	    
	    visited[u] = visited[v] = true;
	    stack.push(v);
	    while (not stack.empty())
	    {
		dfs_start:
		unsigned w = stack.top();
		
		// see if we can go down
		bool shouldStop = false;
		const std::set<unsigned>& adjs = stree.adjs_[w];
		for (std::set<unsigned>::const_iterator x = adjs.begin(); x != adjs.end(); x++)
		    if (*x != u and not visited[*x])
		    {
			visited[*x] = true;
			stack.push(*x);
			goto dfs_start;
		    }
		    else if (stack.size() > 1 and *x == u) // found the circuit!
		    {
			stack.push(*x);
			shouldStop = true;
			break;
		    }
		
		if (shouldStop) break;
		stack.pop();
	    }
	    
	    assert(not stack.empty());
	    //
	    // Seach for edges on fundamental circle end
	    //
	    std::vector<unsigned> cirleVertices;
	    while (not stack.empty())
	    {
		unsigned e = stack.top();
		cirleVertices.push_back(e);
		stack.pop();
	    }
	    
	    for (unsigned s = 0; s < cirleVertices.size() - 1; s++)
	    {
		log(LG_DEBUG, "circuit edge found (%u, %u)", cirleVertices[s], cirleVertices[s+1]);
		assert(stree.isConnected(cirleVertices[s], cirleVertices[s+1]));
		
		stree.removeConnection(cirleVertices[s], cirleVertices[s+1]);
		
		std::vector<PartitionComponent> comps;

		Tree ntree(stree.nVertices() + 1);
		getTreeFromRandomTree(stree, &ntree, false);
	    
		partitionByPerlSchach(ntree, clusters, comps);
		
		assert(clusters == comps.size());
		
		Partition current(comps);

		if (isBetter(current, best))
		{
		    improved++;
		    best = current;
		}
				
		stree.addConnection(cirleVertices[s], cirleVertices[s+1]);
	    }

	    
	    // step back
	    log(LG_DEBUG, "removing edge (%u, %u) to tree", u, v);
	    stree.removeConnection(u, v);
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
    
    log(LG_DEBUG, "cassioRandSpanningTreeHeuristic impoved the original solution %u times", improved);
    return components;
}
#endif
